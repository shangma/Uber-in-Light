#pragma once
#include "Communicator.h"

class SplitScreenCommunicator :
	public Communicator
{
public:
	////////////////////////////// Split to quarters ///////////////////////////
	int sectionsPerLength = 4;	// number of splitting sections
	vector<cv::Rect> ROIs;
	SplitScreenCommunicator(int cellsPerSide)
	{
		sectionsPerLength = cellsPerSide;
	}
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
		int framesForSymbol = (frequency * 1000) / symbol_time;
		/*int sectionsPerLength = sqrt(sections);
		int sectionWidth = Utilities::getFrameSize().width / sectionsPerLength;
		int sectionHeight = Utilities::getFrameSize().height / sectionsPerLength;*/
		// create the video writer
		//ostringstream outputVideoStream;
		//outputVideoStream << msg.size() << "_Split" << sections << Utilities::createOuputVideoName(symbol_time, "image", outputVideoFile);
		VideoWriter vidWriter = Utilities::getVideoWriter("_Split_" + outputVideoFile, framerate, Utilities::getFrameSize());
		// get the sections
		cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
		ROIs = Utilities::getDivisions(sections, 1, false, globalROI,true);
		// add dummy seconds in the beginning of the video
		Utilities::addDummyFramesToVideo(vidWriter, framerate, Utilities::createChessBoard());
		Utilities::addDummyFramesToVideo(vidWriter, framerate);
		for (int i = 0; i < amplitudes1.size();i += (sections * framesForSymbol))
		{
			for (int k = 0; k < framesForSymbol; k++)
			{
				Mat frame = img.clone();
				for (int j = 0; j < sections && (i + (j * framesForSymbol) + k) < amplitudes1.size(); j++)
				{
					// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
					Utilities::updateFrameLuminance(frame, ROIs[j], amplitudes1[i + (j * framesForSymbol) + k]);
				}
				vidWriter << frame;
			}
		}
		// adding one dummy black second to the end of the video
		Utilities::addDummyFramesToVideo(vidWriter, framerate);
		// adding chess board to indicate the end of transmission
		Utilities::addDummyFramesToVideo(vidWriter, framerate, Utilities::createChessBoard());
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
			
			Mat img;
			// create the video writer
			//ostringstream outputVideoStream;
			//outputVideoStream << msg.size() << "_Split" << sections << Utilities::createOuputVideoName(symbol_time, inputVideoFile, outputVideoFile);
			VideoWriter vidWriter = Utilities::getVideoWriter("_Split_" + outputVideoFile, fps, Utilities::getFrameSize());
			int framesForSymbol = (fps * 1000) / symbol_time;

			int inputFrameUsageFrames = fps / framerate;
			// get the sections
			cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
			vector<cv::Rect> ROIs = Utilities::getDivisions(sections, 1, false,globalROI,true);
			// add dummy frames
			videoReader.read(img);
			cv::resize(img, img, Utilities::getFrameSize());
			// add dummy seconds in the beginning of the video
			Utilities::addDummyFramesToVideo(vidWriter, fps, Utilities::createChessBoard());
			Utilities::addDummyFramesToVideo(vidWriter, fps);
			for (int i = 0; i < amplitudes1.size(); i += (sections * framesForSymbol))
			{
				for (int k = 0; k < framesForSymbol; k++)
				{
					if ((i + k)%inputFrameUsageFrames == 0)
					{
						videoReader.read(img);
						cv::resize(img, img, Utilities::getFrameSize());
					}
					Mat frame = img.clone();
					for (int j = 0; j < sections && (i + (j * framesForSymbol) + k) < amplitudes1.size(); j++)
					{
						// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
						Utilities::updateFrameLuminance(frame, ROIs[j], amplitudes1[i + (j * framesForSymbol) + k]);
					}
					vidWriter << frame;
				}
			}
			// add dummy frames
			Utilities::addDummyFramesToVideo(vidWriter, fps);
			// end of sending chess
			Utilities::addDummyFramesToVideo(vidWriter, fps, Utilities::createChessBoard());
		}
		cout << endl;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	///              //////////////      Receive     ///////////////                         ////
	/////////////////////////////////////////////////////////////////////////////////////////////
	vector<short> receiveN(vector<vector<float> > frames, int fps, int frames_per_symbol)
	{
		int sections = sectionsPerLength * sectionsPerLength;
		vector<short> result;
		if (frames.size() == 0)
			return result;
		vector<vector<short>> vt;
		for (int k = 0; k < sections; k++)
		{
			vt.push_back(receive2(frames[k], fps, frames_per_symbol));
		}
		for (int i = 0; i < vt[0].size(); i++)
		{
			for (int j = 0; j < vt.size(); j++)
			{
				result.push_back(vt[j][i]);
			}
		}
		return result;
	}

	// receive with a certain ROI ratio
	vector<short> receive(string fileName, int frames_per_symbol, double ROI_Ratio)
	{
		int fps = 0;
		vector<vector<float> > frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps, sectionsPerLength*sectionsPerLength,true);
		return receiveN(frames, 30, frames_per_symbol);
	}
};

