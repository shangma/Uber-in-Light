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
		vector<int> amplitudes = SendReceive::createWaveGivenFPS(frequency, msg, symbol_time, FREQ[ZERO], FREQ[ONE]);
		// create the video writer
		VideoWriter vidWriter;
		vidWriter.open(outputVideoFile, CV_FOURCC('D', 'I', 'V', 'X'), framerate, cv::Size(frame_width, frame_height));
		for (int i = 0; i < amplitudes.size(); i++)
		{
			Mat frame = img.clone();
			Utilities::updateFrameWithAlpha(frame, cv::Rect(0, 0, frame.cols / 2, frame.rows), amplitudes[i]);
			Utilities::updateFrameWithAlpha(frame, cv::Rect(frame.cols / 2, 0, frame.cols / 2, frame.rows), amplitudes[i]);
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
		vector<int> amplitudes1 = SendReceive::createWaveGivenFPS(frequency, msg, symbol_time, FREQ[ZERO], FREQ[ONE]);
		vector<int> amplitudes2 = SendReceive::createWaveGivenFPS(frequency, msg, symbol_time, FREQ[ONE], FREQ[ZERO]);
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

	/// get video frames luminance (this is the split version which splits the image into two)
	// video name as input
	// percentage of the frame as input (used to get this percentage from the center of the image) and takes value from (0,1]
	// int &framerate: is output parameter
	static vector<float> getVideoFrameLuminances(string videoName, double percent, int &framerate)
	{
		vector<float> frames;
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
		VideoWriter vidWriter;
		vidWriter.open(std::to_string(percent) + ".avi", CV_FOURCC('D', 'I', 'V', 'X'), framerate, cv::Size(width, height));
		cout << count << endl;
		//Mat edges;
		//namedWindow("edges", 1);
		cout << "Processing Frames..." << endl;
		Mat frame;
		cap.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
		cv::Rect ROI1 = cv::Rect(lowerX, lowerY, width / 2 - lowerX, height);
		cv::Rect ROI2 = cv::Rect(lowerX + frame_width/2, lowerY, width / 2 - lowerX, height);
		Mat prev;
		cap.read(prev);
		while (cap.read(frame))
		{
			// save the ROI
			Mat tmp = frame;
			//cv::cvtColor(getImageFFT(frame), tmp, CV_GRAY2BGR);
			vidWriter << tmp;

			Mat hsv1, hsv2;
			vector<Mat> HSV1, HSV2;
			Mat tmp1, tmp2;

			cv::cvtColor(prev, hsv1, CV_BGR2HSV);
			cv::split(hsv1, HSV1);
			HSV1[2].convertTo(tmp1, CV_32F);

			cv::cvtColor(frame, hsv2, CV_BGR2HSV);

			cv::split(hsv2, HSV2);

			HSV2[2].convertTo(tmp2, CV_32F);
			cv::subtract(tmp2, tmp1, tmp);
			// apply the ROI on the two halves 

			float luminance = cv::mean(tmp(ROI1)).val[0];// -cv::mean(tmp(ROI2)).val[0];
			prev = frame.clone();
			//float luminance = getLuminance(tmp, ROI);
			frames.push_back(luminance);

		}
		// the camera will be deinitialized automatically in VideoCapture destructor
		return frames;
	}


	// receive with a certain ROI ratio
	static void receiveWithInputROIRatio(string fileName, int frames_per_symbol, double ROI_Ratio)
	{
		int fps = 0;
		vector<float> frames = getVideoFrameLuminances(fileName, ROI_Ratio, fps);
		SendReceive::receive2(frames, fps, frames_per_symbol);
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
			int frames_per_symbol = (fps * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
			Mat frame;
			// create the video writer
			VideoWriter vidWriter;
			vidWriter.open(outputVideoFile, CV_FOURCC('D', 'I', 'V', 'X'), fps, cv::Size(frame_width, frame_height));
			int inputFrameUsageFrames = fps / framerate;
			int InputFrameCounter = 0;
			for (int i = 0; i < msg.length(); i++)
			{
				for (int j = 7; j >= 0; j--)
				{
					int needed_frequency = FREQ[(msg[i] >> (7 - j)) & 1];
					int frames_per_half_cycle = fps / (needed_frequency * 2);
					// start high
					int luminance_index = 0;
					for (int k = 0; k < frames_per_symbol; k++, InputFrameCounter++)
					{
						if ((InputFrameCounter%inputFrameUsageFrames) == 0)
						{
							if (!videoReader.read(frame))
							{
								InputFrameCounter--;
								break;
							}
						}
						if ((k%frames_per_half_cycle) == 0)
						{
							luminance_index ^= 1;
						}
						Mat tmp = frame.clone();
						Utilities::updateFrameWithAlpha(frame, cv::Rect(0, 0, frame.cols / 2, frame.rows), LUMINANCE[luminance_index]);
						Utilities::updateFrameWithAlpha(frame, cv::Rect(frame.cols / 2, 0, frame.cols / 2, frame.rows), LUMINANCE[luminance_index ^ 1]);
						vidWriter << tmp;
					}
					cout << (int)((msg[i] >> (7 - j)) & 1);
				}
			}

		}
		cout << endl;
	}

};
