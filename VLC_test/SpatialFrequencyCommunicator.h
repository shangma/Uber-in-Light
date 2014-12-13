#pragma once

#include "Utilities.h"
#include "Communicator.h"

class SpatialFrequencyCommunicator : public Communicator
{
public:
	// symbol_time: how many milliseconds will the symbol last
	void sendImage(double frequency, string inputImage, string msg, string outputVideoFile, int symbol_time)
	{
		Mat img = imread(inputImage);

		int framerate = frequency; //get the frame rate
		int frame_width = img.cols;
		int frame_height = img.rows;
		// create the video writer
		VideoWriter vidWriter = Utilities::getVideoWriter("SpatialFrequency_" + outputVideoFile, framerate, cv::Size(frame_width, frame_height));
		if (vidWriter.isOpened())
		{
			Mat frame = img.clone();
			int clnWidth = frame_width / 20;
			for (int i = 0; i < frame_width - clnWidth; i += clnWidth)
			{
				Utilities::updateFrameLuminance(frame, cv::Rect(i, 0, clnWidth / 2, frame_height), LUMINANCE[0]);
				Utilities::updateFrameLuminance(frame, cv::Rect(i + clnWidth / 2, 0, clnWidth / 2, frame_height), LUMINANCE[1]);
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
	}
	// symbol_time: how many milliseconds will the symbol last
	void sendVideo(string inputVideoFile, string msg, string outputVideoFile, int symbol_time)
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
				frame_height >>= 1;
				frame_width >>= 1;
			}
			VideoWriter vidWriter;
			if (vidWriter.open("SpatialFrequency_" + outputVideoFile, 0/*CV_FOURCC('D', 'I', 'V', 'X')*/, framerate, Utilities::getFrameSize()))
			{
				Mat frame;

				int clnWidth = frame_width / 20;
				
				for (int j = 0; j < 300; j++)
				{
					typedef std::chrono::high_resolution_clock Clock;
					typedef std::chrono::milliseconds milliseconds;
					Clock::time_point t0 = Clock::now();
					videoReader.read(frame);
					cv::resize(frame, frame, Utilities::getFrameSize());
					if (j % 2)
					{
						for (int i = 0; i < frame_width - clnWidth; i += clnWidth)
						{
							Utilities::updateFrameLuminance(frame, cv::Rect(i, 0, clnWidth / 2, frame_height), LUMINANCE[0]);
							Utilities::updateFrameLuminance(frame, cv::Rect(i + clnWidth / 2, 0, clnWidth / 2, frame_height), LUMINANCE[1]);
						}
					}
					vidWriter << frame;
					//imshow("frame", frame);
					Clock::time_point t1 = Clock::now();
					cout << std::chrono::duration_cast<milliseconds>(t1 - t0).count() << endl;
					//cv::waitKey(0);
				}
			}
		}
		cout << endl;
	}

	// receive with a certain ROI ratio
	vector<short> receive(string fileName, int frames_per_symbol, double ROI_Ratio)
	{
		vector<short> result;
		VideoCapture cap(fileName); // open the default camera
		if (cap.isOpened())  // check if we succeeded
		{
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
				Mat tmp = Utilities::getDiffInVchannelHSV(prev, frame, 0);
				imshow("test", tmp);
				cvWaitKey(0);
				prev = frame.clone();
			}
		}
		return result;
	}
};