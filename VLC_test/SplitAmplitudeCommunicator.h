#pragma once
#include "Communicator.h"
class SplitAmplitudeCommunicator :
	public Communicator
{
public:
	////////////////////////////// Split Amplitude ///////////////////////////

	// symbol_time: how many milliseconds will the symbol last
	void sendImage(double frequency, string inputImage, vector<short> msg, string outputVideoFile, int symbol_time)
	{
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
		// create the video writer
		//ostringstream outputVideoStream;
		//outputVideoStream << msg.size() << "_AmpDiff" << Utilities::createOuputVideoName(symbol_time, "image", outputVideoFile);
		VideoWriter vidWriter = Utilities::getVideoWriter("_AmpDiff_" + outputVideoFile, framerate, Utilities::getFrameSize());
		Utilities::addDummyFramesToVideo(vidWriter, framerate, Utilities::createChessBoard());
		Utilities::addDummyFramesToVideo(vidWriter, framerate);
		cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
		vector<Rect> ROIs = Utilities::getDivisions(2, 1, false, globalROI,true);
		for (int i = 0; i < amplitudes1.size();i++)
		{
			Mat frame = img.clone();
			Utilities::updateFrameLuminance(frame, ROIs[0], amplitudes1[i]);
			Utilities::updateFrameLuminance(frame, ROIs[1], amplitudes2[i]);
			vidWriter << frame;
		}
		Utilities::addDummyFramesToVideo(vidWriter, framerate);
		Utilities::addDummyFramesToVideo(vidWriter, framerate, Utilities::createChessBoard());		
	}

	// symbol_time: how many milliseconds will the symbol last
	void sendVideo(string inputVideoFile, vector<short> msg, string outputVideoFile, int symbol_time)
	{
		VideoCapture videoReader(inputVideoFile);
		if (videoReader.isOpened())
		{
			videoReader.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
			int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
			int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
			while (frame_height > 1000)
			{
				frame_width /= 2;
				frame_height /= 2;
			}
			int fps = Utilities::lcm((int)framerate, Utilities::lcm(2 * FREQ[ZERO], 2 * FREQ[ONE]));
			//int frames_per_symbol = (fps * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
			double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
			vector<float> amplitudes1 = createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1);
			double lumin2[] = { LUMINANCE[1], LUMINANCE[0] };
			vector<float> amplitudes2 = createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin2);
			Mat frame;
			
			// create the video writer
			//ostringstream outputVideoStream;
			//outputVideoStream << msg.size() << "_AmpDiff" << Utilities::createOuputVideoName(symbol_time, inputVideoFile, outputVideoFile);
			VideoWriter vidWriter = Utilities::getVideoWriter("_AmpDiff_" + outputVideoFile, fps, Utilities::getFrameSize());
			int inputFrameUsageFrames = fps / framerate;
			videoReader.read(frame);
			Utilities::addDummyFramesToVideo(vidWriter, fps, Utilities::createChessBoard());
			Utilities::addDummyFramesToVideo(vidWriter, fps);
			cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
			vector<Rect> ROIs = Utilities::getDivisions(2, 1, false, globalROI, true);
			for (int k = 0; k < amplitudes1.size(); k++)
			{
				if (k%inputFrameUsageFrames == 0)
				{
					videoReader.read(frame);
				}
				Mat tmp;
				cv::resize(frame, tmp, Utilities::getFrameSize());
				Utilities::updateFrameLuminance(tmp, ROIs[0], amplitudes1[k]);
				Utilities::updateFrameLuminance(tmp, ROIs[1], amplitudes2[k]);
				vidWriter << tmp;
			}
			Utilities::addDummyFramesToVideo(vidWriter, fps);
			Utilities::addDummyFramesToVideo(vidWriter, fps, Utilities::createChessBoard());
		}
		cout << endl;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	///              //////////////      Receive     ///////////////                         ////
	/////////////////////////////////////////////////////////////////////////////////////////////


	// receive with a certain ROI ratio
	vector<short> receive(string fileName, int frames_per_symbol, double ROI_Ratio)
	{
		int fps = 0;
		vector<vector<float> > frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps, 2,true);
		vector<float> amplitude_difference;
		for (int i = 0; i < frames[0].size(); i++)
		{
			amplitude_difference.push_back(frames[0][i] - frames[1][i]);
		}
		return receive2(amplitude_difference, 30, frames_per_symbol);
	}
};

