#pragma once

#include "SplitScreenCommunicator.h"

class SplitScreenAmpDifferenceCommunicator :
	public SplitScreenCommunicator
{
public:
	////////////////////////////// Split to quarters ///////////////////////////
	SplitScreenAmpDifferenceCommunicator(int cellsPerSide) : SplitScreenCommunicator(cellsPerSide){}
	
	virtual string getVideoName(string outputVideoFile)
	{
		return "_SplitAmp_" + outputVideoFile;
	}
	
	virtual void initCommunication()
	{
		double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1));
		double lumin2[] = { LUMINANCE[1], LUMINANCE[0] };
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin2));
		
		framesForSymbol = (fps * 1000) / symbol_time;
		 
		ROIs = Utilities::getDivisions(sections, 1, false, globalROI, true);
	}
	virtual void sendImageMainLoop()
	{
		for (int i = 0; i < amplitudes[0].size(); i += (sections * framesForSymbol))
		{
			for (int k = 0; k < framesForSymbol; k++)
			{
				Mat frame = img.clone();
				for (int j = 0; j < sections && (i + (j * framesForSymbol) + k) < amplitudes[0].size(); j++)
				{
					// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
					Utilities::updateFrameLuminance(frame, cv::Rect(ROIs[j].x, ROIs[j].y, ROIs[j].width / 2, ROIs[j].height),
						amplitudes[0][i + (j * framesForSymbol) + k]);
					Utilities::updateFrameLuminance(frame, cv::Rect(ROIs[j].x + ROIs[j].width / 2, ROIs[j].y, ROIs[j].width / 2, ROIs[j].height),
						amplitudes[1][i + (j * framesForSymbol) + k]);
				}
				vidWriter << frame;
			}
		}
	}
	virtual void sendVideoMainLoop()
	{
		for (int i = 0; i < amplitudes[0].size(); i += (sections * framesForSymbol))
		{
			for (int k = 0; k < framesForSymbol; k++)
			{
				if ((i + k) % inputFrameUsageFrames == 0)
				{
					videoReader.read(img);
					cv::resize(img, img, Utilities::getFrameSize());
				}
				Mat frame = img.clone();
				for (int j = 0; j < sections && (i + (j * framesForSymbol) + k) < amplitudes[0].size(); j++)
				{
					// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
					Utilities::updateFrameLuminance(frame, cv::Rect(ROIs[j].x, ROIs[j].y, ROIs[j].width / 2, ROIs[j].height),
						amplitudes[0][i + (j * framesForSymbol) + k]);
					Utilities::updateFrameLuminance(frame, cv::Rect(ROIs[j].x + ROIs[j].width / 2, ROIs[j].y, ROIs[j].width / 2, ROIs[j].height),
						amplitudes[1][i + (j * framesForSymbol) + k]);
				}
				vidWriter << frame;
			}
		}
	}

	// symbol_time: how many milliseconds will the symbol last
	//void sendImage()
	//{
	//
	//	//Mat img = imread(inputImage);
	//	//cv::resize(img, img, Utilities::getFrameSize());
	//	//int framerate = frequency; //get the frame rate
	//	//int frame_width = img.cols;
	//	//int frame_height = img.rows;
	//	//int frames_per_symbol = (framerate * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
	//	double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
	//	vector<float> amplitudes1 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1);
	//	double lumin2[] = { LUMINANCE[1], LUMINANCE[0] };
	//	vector<float> amplitudes2 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin2);
	//	framesForSymbol = (fps * 1000) / symbol_time;
	//	// create the video writer
	//	//ostringstream outputVideoStream;
	//	//outputVideoStream << msg.size() << "_SplitAmp" << sections << Utilities::createOuputVideoName(symbol_time, "image", outputVideoFile);
	//	//VideoWriter vidWriter = Utilities::getVideoWriter("_SplitAmp_" + outputVideoFile, fps, Utilities::getFrameSize());
	//	// get the sections
	//	//cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
	//	ROIs = Utilities::getDivisions(sections, 1, false,globalROI,true);
	//	// add dummy seconds in the beginning of the video
	//	addSynchFrames(false);
	//	for (int i = 0; i < amplitudes1.size(); i += (sections * framesForSymbol))
	//	{
	//		for (int k = 0; k < framesForSymbol; k++)
	//		{
	//			Mat frame = img.clone();
	//			for (int j = 0; j < sections && (i + (j * framesForSymbol) + k) < amplitudes1.size(); j++)
	//			{
	//				// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
	//				Utilities::updateFrameLuminance(frame, cv::Rect(ROIs[j].x, ROIs[j].y, ROIs[j].width / 2, ROIs[j].height),
	//					amplitudes1[i + (j * framesForSymbol) + k]);
	//				Utilities::updateFrameLuminance(frame, cv::Rect(ROIs[j].x + ROIs[j].width / 2, ROIs[j].y, ROIs[j].width / 2, ROIs[j].height),
	//					amplitudes2[i + (j * framesForSymbol) + k]);
	//			}
	//			vidWriter << frame;
	//		}
	//	}
	//	// adding one dummy black second to the end of the video
	//	addSynchFrames(true);
	//}

	// symbol_time: how many milliseconds will the symbol last
	//void sendVideo(/*string inputVideoFile, vector<short> msg, string outputVideoFile, int symbol_time*/)
	//{
	//	//cout << "I am here\n";
	//	//VideoCapture videoReader(inputVideoFile);
	//	if (videoReader.isOpened())
	//	{
	//		//videoReader.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
	//		//int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
	//		//int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
	//		//int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
	//		//int fps = Utilities::getOuputVideoFrameRate((int)framerate);
	//		//int frames_per_symbol = (fps * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
	//		double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
	//		vector<float> amplitudes1 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1);
	//		double lumin2[] = { LUMINANCE[1], LUMINANCE[0] };
	//		vector<float> amplitudes2 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin2);
	//		Mat img;
	//		// create the video writer
	//		//ostringstream outputVideoStream;
	//		//outputVideoStream << msg.size() << "_SplitAmp" << sections << Utilities::createOuputVideoName(symbol_time, inputVideoFile, outputVideoFile);
	//		//VideoWriter vidWriter = Utilities::getVideoWriter("_SplitAmp_" + outputVideoFile, fps, Utilities::getFrameSize());
	//		framesForSymbol = (fps * 1000) / symbol_time;

	//		//int inputFrameUsageFrames = fps / framerate;
	//		// get the sections
	//		//cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
	//		ROIs = Utilities::getDivisions(sections, 1, false,globalROI,true);
	//		// add dummy frames
	//		videoReader.read(img);
	//		cv::resize(img, img, Utilities::getFrameSize());
	//		addSynchFrames(false);
	//		for (int i = 0; i < amplitudes1.size(); i += (sections * framesForSymbol))
	//		{
	//			for (int k = 0; k < framesForSymbol; k++)
	//			{
	//				if ((i + k) % inputFrameUsageFrames == 0)
	//				{
	//					videoReader.read(img);
	//					cv::resize(img, img, Utilities::getFrameSize());
	//				}
	//				Mat frame = img.clone();
	//				for (int j = 0; j < sections && (i + (j * framesForSymbol) + k) < amplitudes1.size(); j++)
	//				{
	//					// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
	//					Utilities::updateFrameLuminance(frame, cv::Rect(ROIs[j].x, ROIs[j].y, ROIs[j].width / 2, ROIs[j].height),
	//						amplitudes1[i + (j * framesForSymbol) + k]);
	//					Utilities::updateFrameLuminance(frame, cv::Rect(ROIs[j].x + ROIs[j].width / 2, ROIs[j].y, ROIs[j].width / 2, ROIs[j].height),
	//						amplitudes2[i + (j * framesForSymbol) + k]);
	//				}
	//				vidWriter << frame;
	//			}
	//		}
	//		addSynchFrames(true);
	//	}
	//	cout << endl;
	//}

	// receive with a certain ROI ratio
	vector<short> receive(string fileName, int frames_per_symbol, double ROI_Ratio)
	{
		vector<short> results;
		VideoCapture cap(fileName); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
			return results;
		double framerate = cap.get(CV_CAP_PROP_FPS); //get the frame rate
		// try to detect the chess board
		int index = 0;
		cv::Rect globalROI(0, 0, cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));
		globalROI = Utilities::getGlobalROI(fileName, index);
		cap.set(CV_CAP_PROP_POS_FRAMES, index);
		cout << "Index = " << index << endl;
		ROIs = Utilities::getDivisions(sectionsPerLength*sectionsPerLength, ROI_Ratio, false,globalROI,false);
		vector<cv::Rect> ROIs2;
		for (int i = 0; i < ROIs.size(); i++)
		{
			ROIs2.push_back(cv::Rect(ROIs[i].x, ROIs[i].y, ROIs[i].width / 2, ROIs[i].height));
			ROIs2.push_back(cv::Rect(ROIs[i].x + ROIs[i].width / 2, ROIs[i].y, ROIs[i].width / 2, ROIs[i].height));
		}
		vector<vector<float> > frames2 = Utilities::getVideoFrameLuminances(cap, ROIs2, framerate, true, globalROI);
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

