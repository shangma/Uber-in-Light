#pragma once

#include "Utilities.h"
#include "SendReceive.h"

class SpatialFrequency
{
public:
	// symbol_time: how many milliseconds will the symbol last
	static void createOfflineVideoWithGivenFPS(double frequency, string inputImage, string msg, string outputVideoFile, int symbol_time)
	{
		Mat img = imread(inputImage);

		int framerate = frequency; //get the frame rate
		int frame_width = img.cols;
		int frame_height = img.rows;
		// create the video writer
		VideoWriter vidWriter;
		vidWriter.open("SpatialFrequency_" + outputVideoFile, CV_FOURCC('D', 'I', 'V', 'X'), framerate, cv::Size(frame_width, frame_height));
		Mat frame = img.clone();
		for (int i = 0; i < frame_width - 20; i += 20)
		{
			Utilities::updateFrameWithAlpha(frame, cv::Rect(i, 0, 10, frame_height), LUMINANCE[0]);
			Utilities::updateFrameWithAlpha(frame, cv::Rect(i + 10, 0, 10, frame_height), LUMINANCE[1]);
		}
		for (int i = 0; i < 300; i++)
		{
			if (i % 2)
			{
				vidWriter << frame;
			}
			else
			{
				vidWriter << img;
			}
		}
	}

	/// get video frames luminance (this is the split version which splits the image into two)
	// video name as input
	// percentage of the frame as input (used to get this percentage from the center of the image) and takes value from (0,1]
	// int &framerate: is output parameter
	static void getVideoFrameLuminances(string videoName)
	{
		VideoCapture cap(videoName); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
			return;
		double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
		int framerate = cap.get(CV_CAP_PROP_FPS); //get the frame rate
		int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
		int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
		// the ROI
		Mat prev, frame;
		cap.read(prev);
		while (cap.read(frame))
		{
			// save the ROI
			Mat tmp,hsv1, hsv2;
			vector<Mat> HSV1, HSV2;
			Mat tmp1, tmp2;

			cv::cvtColor(prev, hsv1, CV_BGR2HSV);
			cv::split(hsv1, HSV1);
			HSV1[2].convertTo(tmp1, CV_32F);

			cv::cvtColor(frame, hsv2, CV_BGR2HSV);

			cv::split(hsv2, HSV2);

			HSV2[2].convertTo(tmp2, CV_32F);
			cv::subtract(tmp2, tmp1, tmp);
			imshow("test", tmp * 255);
			cvWaitKey(0);
			prev = frame.clone();
		}

	}
};