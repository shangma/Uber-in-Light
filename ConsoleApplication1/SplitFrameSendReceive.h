#pragma once

#include "Utilities.h"

class SplitFrameSendReceive
{
public:

	// symbol_time: how many milliseconds will the symbol last
	static void createOfflineVideoWithGivenFPS(double frequency, string inputImage, string msg, string outputVideoFile, int symbol_time)
	{
		Mat img = imread(inputImage);

		int framerate = frequency; //get the frame rate
		int frame_width = img.cols;
		int frame_height = img.rows;
		//int frames_per_symbol = (framerate * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
		int lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
		vector<int> amplitudes1 = SendReceive::createWaveGivenFPS(frequency, msg, symbol_time, FREQ[ZERO], FREQ[ONE],lumin1);
		int lumin2[] = { LUMINANCE[1], LUMINANCE[0] };
		vector<int> amplitudes2 = SendReceive::createWaveGivenFPS(frequency, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin2);
		// create the video writer
		VideoWriter vidWriter;
		vidWriter.open(outputVideoFile, CV_FOURCC('D', 'I', 'V', 'X'), framerate, cv::Size(frame_width, frame_height));
		for (int i = 0; i < amplitudes1.size(); i++)
		{
			Mat frame = img.clone();
			Utilities::updateFrameWithAlpha(frame, cv::Rect(0, 0, frame.cols / 2, frame.rows), amplitudes1[i]);
			Utilities::updateFrameWithAlpha(frame, cv::Rect(frame.cols / 2, 0, frame.cols / 2, frame.rows), amplitudes2[i]);
			vidWriter << frame;
		}
	}


	// symbol_time: how many milliseconds will the symbol last
	static void createOfflineVideoWithGivenFPSWithTwoFreq(double frequency, string inputImage, string msg, string outputVideoFile, int symbol_time)
	{
		Mat img = imread(inputImage);

		int framerate = frequency; //get the frame rate
		int frame_width = img.cols;
		int frame_height = img.rows;
		//int frames_per_symbol = (framerate * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
		int lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
		vector<int> amplitudes1 = SendReceive::createWaveGivenFPS(frequency, msg, symbol_time, FREQ[ZERO], FREQ[ONE],lumin1);
		vector<int> amplitudes2 = SendReceive::createWaveGivenFPS(frequency, msg, symbol_time, FREQ[ONE], FREQ[ZERO],lumin1);
		// create the video writer
		VideoWriter vidWriter;
		vidWriter.open(outputVideoFile, CV_FOURCC('D', 'I', 'V', 'X'), framerate, cv::Size(frame_width, frame_height));
		for (int i = 0; i < amplitudes1.size(); i++)
		{
			Mat frame = img.clone();
			Utilities::updateFrameWithAlpha(frame, cv::Rect(0, 0, frame.cols / 2, frame.rows), amplitudes1[i]);
			Utilities::updateFrameWithAlpha(frame, cv::Rect(frame.cols / 2, 0, frame.cols / 2, frame.rows), amplitudes2[i]);
			vidWriter << frame;
		}
	}

	// symbol_time: how many milliseconds will the symbol last
	static void createOfflineVideoFromVideo(string inputVideoFile, string msg, string outputVideoFile, int symbol_time)
	{
		VideoCapture videoReader(inputVideoFile);
		if (videoReader.isOpened())
		{
			videoReader.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
			int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
			int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
			int fps = Utilities::lcm((int)framerate, Utilities::lcm(2 * FREQ[ZERO], 2 * FREQ[ONE]));
			//int frames_per_symbol = (fps * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
			int lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
			vector<int> amplitudes1 = SendReceive::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1);
			int lumin2[] = { LUMINANCE[1], LUMINANCE[0] };
			vector<int> amplitudes2 = SendReceive::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin2);
			Mat frame;
			// create the video writer
			VideoWriter vidWriter;
			vidWriter.open(outputVideoFile, CV_FOURCC('D', 'I', 'V', 'X'), fps, cv::Size(frame_width, frame_height));
			int inputFrameUsageFrames = fps / framerate;
			for (int k = 0; k < amplitudes1.size(); k++)
			{
				if (k%inputFrameUsageFrames == 0)
				{
					videoReader.read(frame);
				}
				Mat tmp = frame.clone();
				Utilities::updateFrameWithAlpha(frame, cv::Rect(0, 0, frame.cols / 2, frame.rows), amplitudes1[k]);
				Utilities::updateFrameWithAlpha(frame, cv::Rect(frame.cols / 2, 0, frame.cols / 2, frame.rows), amplitudes2[k]);
				vidWriter << tmp;

			}
		}
		cout << endl;
	}

	// symbol_time: how many milliseconds will the symbol last
	static void createOfflineVideoFromVideoWithTwoFreq(string inputVideoFile, string msg, string outputVideoFile, int symbol_time)
	{
		VideoCapture videoReader(inputVideoFile);
		if (videoReader.isOpened())
		{
			videoReader.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
			int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
			int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
			int fps = Utilities::lcm((int)framerate, Utilities::lcm(2 * FREQ[ZERO], 2 * FREQ[ONE]));
			//int frames_per_symbol = (fps * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
			int lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
			vector<int> amplitudes1 = SendReceive::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1);
			vector<int> amplitudes2 = SendReceive::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ONE], FREQ[ZERO], lumin1);
			Mat frame;
			// create the video writer
			VideoWriter vidWriter;
			vidWriter.open(outputVideoFile, CV_FOURCC('D', 'I', 'V', 'X'), fps, cv::Size(frame_width, frame_height));
			int inputFrameUsageFrames = fps / framerate;
			for (int k = 0; k < amplitudes1.size(); k++)
			{
				if (k%inputFrameUsageFrames == 0)
				{
					videoReader.read(frame);
				}
				Mat tmp = frame.clone();
				Utilities::updateFrameWithAlpha(frame, cv::Rect(0, 0, frame.cols / 2, frame.rows), amplitudes1[k]);
				Utilities::updateFrameWithAlpha(frame, cv::Rect(frame.cols / 2, 0, frame.cols / 2, frame.rows), amplitudes2[k]);
				vidWriter << tmp;

			}
		}
		cout << endl;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	///              //////////////      Receive     ///////////////                         ////
	/////////////////////////////////////////////////////////////////////////////////////////////

	/// get video frames luminance (this is the split version which splits the image into two)
	// video name as input
	// percentage of the frame as input (used to get this percentage from the center of the image) and takes value from (0,1]
	// int &framerate: is output parameter
	static vector<vector<float> > getVideoFrameLuminances(string videoName, double percent, int &framerate)
	{
		vector<vector<float> > frames;
		VideoCapture cap(videoName); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
			return frames;
		double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
		framerate = cap.get(CV_CAP_PROP_FPS); //get the frame rate
		int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
		int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
		// the ROI
		int width = frame_width * percent;
		int height = frame_height * percent;
		int lowerX = (frame_width - width) / 2;
		int lowerY = (frame_height - height) / 2;

		cv::Rect ROI1 = cv::Rect(lowerX, lowerY, width / 2 - lowerX, height);
		cv::Rect ROI2 = cv::Rect(lowerX + frame_width / 2, lowerY, width / 2 - lowerX, height);

		cap.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
		frames.push_back(SendReceive::getVideoFrameLuminances(cap, ROI1));
		cap.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
		frames.push_back(SendReceive::getVideoFrameLuminances(cap, ROI2));

		return frames;
	}


	// receive with a certain ROI ratio
	static void receiveWithInputROIRatioAmplitudeDiff(string fileName, int frames_per_symbol, double ROI_Ratio)
	{
		int fps = 0;
		vector<vector<float> > frames = getVideoFrameLuminances(fileName, ROI_Ratio, fps);
		vector<float> amplitude_difference;
		for (int i = 0; i < frames[0].size(); i++)
		{
			amplitude_difference.push_back(frames[0][i] - frames[1][i]);
		}
		SendReceive::receive2(amplitude_difference, fps, frames_per_symbol);
	}

	// receive with a certain ROI ratio
	static void receiveWithInputROIRatioFreqDiff(string fileName, int frames_per_symbol, double ROI_Ratio)
	{
		int fps = 0;
		vector<vector<float> > frames = getVideoFrameLuminances(fileName, ROI_Ratio, fps);
		double sum = 0;
		vector<float> freqDiff;
		vector<float> freqDiffAvg;
		for (int i = 0; i < frames[0].size() - frames_per_symbol; i++)
		{
			vector<Frequency> temp0 = Utilities::myft(frames[0], fps, i, frames_per_symbol);
			vector<Frequency> temp1 = Utilities::myft(frames[1], fps, i, frames_per_symbol);
			// get the difference between maximums
			int max0 = 0, max1 = 0;
			for (int j = 1; j < temp0.size(); j++)
			{
				if (temp0[j].percent > temp0[max0].percent)
				{
					max0 = j;
				}
				if (temp1[j].percent > temp1[max1].percent)
				{
					max1 = j;
				}
			}
			freqDiff.push_back(temp0[max0].freq - temp1[max1].freq);
			sum += freqDiff[i];
			if (i >= frames_per_symbol)
			{
				sum -= freqDiff[i - frames_per_symbol];
				freqDiffAvg.push_back(sum / frames_per_symbol);
			}
		}
		// output the numbers
		int ind = 0;
		for (; ind < freqDiffAvg.size(); ind++)
		{
			if (abs(freqDiff[ind] - (FREQ[ZERO] - FREQ[ONE])) < EPSILON)
			{
				printf("0");
				break;
			}
			else if (abs(freqDiff[ind] - (FREQ[ONE] - FREQ[ZERO])) < EPSILON)
			{
				printf("1");
				break;
			}
		}
		for (; ind < freqDiffAvg.size();)
		{
			if (abs(freqDiff[ind] - (FREQ[ZERO] - FREQ[ONE])) < EPSILON)
			{
				printf("0");
				ind += frames_per_symbol;
			}
			else if (abs(freqDiff[ind] - (FREQ[ONE] - FREQ[ZERO])) < EPSILON)
			{
				printf("1");
				ind += frames_per_symbol;
			}
			else
			{
				ind++;
			}
		}
	}
};
