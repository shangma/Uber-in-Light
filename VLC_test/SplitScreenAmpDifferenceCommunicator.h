#pragma once

#include "SplitScreenCommunicator.h"

class SplitScreenAmpDifferenceCommunicator :
	public SplitScreenCommunicator
{
public:
	////////////////////////////// Split to quarters ///////////////////////////
	SplitScreenAmpDifferenceCommunicator(int cellsPerSide) : SplitScreenCommunicator(cellsPerSide){}
	// symbol_time: how many milliseconds will the symbol last
	void sendImage(double frequency, string inputImage, vector<short> msg, string outputVideoFile, int symbol_time)
	{
		int sections = sectionsPerLength * sectionsPerLength;
		Mat img = imread(inputImage);
		cv::resize(img, img, Utilities::getFrameSize());
		int framerate = frequency; //get the frame rate
		int frame_width = img.cols;
		int frame_height = img.rows;
		//int frames_per_symbol = (framerate * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
		double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
		vector<float> amplitudes1 = createWaveGivenFPS(frequency, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1);
		double lumin2[] = { LUMINANCE[1], LUMINANCE[0] };
		vector<float> amplitudes2 = createWaveGivenFPS(frequency, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin2);
		int framesForSymbol = (frequency * 1000) / symbol_time;
		// create the video writer
		ostringstream outputVideoStream;
		outputVideoStream << msg.size() << "_SplitAmp" << sections << Utilities::createOuputVideoName(symbol_time, "image", outputVideoFile);
		VideoWriter vidWriter = Utilities::getVideoWriter(outputVideoStream.str(), framerate, Utilities::getFrameSize());
		// get the sections
		cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
		ROIs = Utilities::getDivisions(sections, frame_width, frame_height, 1, false,globalROI);
		// add dummy seconds in the beginning of the video
		Utilities::addDummyFramesToVideo(vidWriter, framerate, img.clone() * 0);
		for (int i = 0; i < amplitudes1.size(); i += (sections * framesForSymbol))
		{
			for (int k = 0; k < framesForSymbol; k++)
			{
				Mat frame = img.clone();
				for (int j = 0; j < sections; j++)
				{
					// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
					Utilities::updateFrameWithAlpha(frame, cv::Rect(ROIs[j].x,ROIs[j].y,ROIs[j].width/2,ROIs[j].height), 
						amplitudes1[i + (j * framesForSymbol) + k]);
					Utilities::updateFrameWithAlpha(frame, cv::Rect(ROIs[j].x + ROIs[j].width / 2, ROIs[j].y, ROIs[j].width / 2, ROIs[j].height), 
						amplitudes2[i + (j * framesForSymbol) + k]);
				}
				vidWriter << frame;
			}
		}
		// adding one dummy black second to the end of the video
		Utilities::addDummyFramesToVideo(vidWriter, framerate, img.clone() * 0);
	}

	// symbol_time: how many milliseconds will the symbol last
	void sendVideo(string inputVideoFile, vector<short> msg, string outputVideoFile, int symbol_time)
	{
		cout << "I am here\n";
		int sections = sectionsPerLength * sectionsPerLength;
		VideoCapture videoReader(inputVideoFile);
		if (videoReader.isOpened())
		{
			videoReader.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
			int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
			int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
			int fps = Utilities::lcm((int)framerate, Utilities::lcm(2 * FREQ[ZERO], 2 * FREQ[ONE]));
			//int frames_per_symbol = (fps * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
			double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
			vector<float> amplitudes1 = createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1);
			double lumin2[] = { LUMINANCE[1], LUMINANCE[0] };
			vector<float> amplitudes2 = createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin2);
			Mat img;
			// create the video writer
			ostringstream outputVideoStream;
			outputVideoStream << msg.size() << "_SplitAmp" << sections << Utilities::createOuputVideoName(symbol_time, inputVideoFile, outputVideoFile);
			VideoWriter vidWriter = Utilities::getVideoWriter(outputVideoStream.str(), fps, Utilities::getFrameSize());
			int framesForSymbol = (fps * 1000) / symbol_time;

			int inputFrameUsageFrames = fps / framerate;
			// get the sections
			cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
			ROIs = Utilities::getDivisions(sections, Utilities::getFrameSize().width, Utilities::getFrameSize().height, 1, false,globalROI);
			// add dummy frames
			videoReader.read(img);
			cv::resize(img, img, Utilities::getFrameSize());
			Utilities::addDummyFramesToVideo(vidWriter, fps, img.clone() * 0);
			for (int i = 0; i < amplitudes1.size(); i += (sections * framesForSymbol))
			{
				for (int k = 0; k < framesForSymbol; k++)
				{
					if ((i + k) % inputFrameUsageFrames == 0)
					{
						videoReader.read(img);
						cv::resize(img, img, Utilities::getFrameSize());
					}
					Mat frame = img.clone();
					for (int j = 0; j < sections; j++)
					{
						// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
						Utilities::updateFrameWithAlpha(frame, cv::Rect(ROIs[j].x, ROIs[j].y, ROIs[j].width / 2, ROIs[j].height),
							amplitudes1[i + (j * framesForSymbol) + k]);
						Utilities::updateFrameWithAlpha(frame, cv::Rect(ROIs[j].x + ROIs[j].width / 2, ROIs[j].y, ROIs[j].width / 2, ROIs[j].height),
							amplitudes2[i + (j * framesForSymbol) + k]);
					}
					vidWriter << frame;
				}
			}
			// add dummy frames
			Utilities::addDummyFramesToVideo(vidWriter, fps, img.clone() * 0);
		}
		cout << endl;
	}

	// receive with a certain ROI ratio
	vector<short> receive(string fileName, int frames_per_symbol, double ROI_Ratio)
	{
		vector<short> results;
		VideoCapture cap(fileName); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
			return results;
		double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
		int fps = cap.get(CV_CAP_PROP_FPS); //get the frame rate
		int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
		int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
		// try to detect the chess board
		int index = 0;
		Mat frame;
		cv::Rect globalROI;
		for (; cap.read(frame); index++)
		{
			// this loop to detect the first chess board
			globalROI = Utilities::detectMyBoard(frame);
			if (globalROI.width > 10 && globalROI.height > 10)
			{
				break;
			}
		}
		int countChess = 0;
		for (; cap.read(frame); index++, countChess++)
		{
			// this loop to detect the last chess board
			Rect roi = Utilities::detectMyBoard(frame);
			if (roi.width < 10 || roi.height < 10)
			{
				break;
			}
			globalROI = roi;
		}
		ROIs = Utilities::getDivisions(sectionsPerLength*sectionsPerLength, frame_width, frame_height, ROI_Ratio, false,globalROI);
		vector<cv::Rect> ROIs2;
		for (int i = 0; i < ROIs.size(); i++)
		{
			ROIs2.push_back(cv::Rect(ROIs[i].x, ROIs[i].y, ROIs[i].width / 2, ROIs[i].height));
			ROIs2.push_back(cv::Rect(ROIs[i].x + ROIs[i].width / 2, ROIs[i].y, ROIs[i].width / 2, ROIs[i].height));
		}
		vector<vector<float> > frames2 = Utilities::getVideoFrameLuminances(cap, ROIs2, fps);
		vector<vector<float> > frames(ROIs.size());
		for (int i = 0; i < frames2.size(); i += 2)
		{
			for (int j = 0; j < frames2[i].size(); j++)
			{
				frames[i / 2].push_back(frames2[i][j] - frames2[i + 1][j]);
			}
		}
		results = receiveN(frames, 30, frames_per_symbol);
		return results;
	}
};

