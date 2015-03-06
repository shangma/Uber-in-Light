#pragma once
#include "Communicator.h"
class BGRCommunicator2 :
	public Communicator
{
public:
	////////////////////////////// Split Amplitude ///////////////////////////
	virtual string getVideoName(string outputVideoFile)
	{
		return "_RGB2_" + outputVideoFile;
	}
	virtual void initCommunication()
	{
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(msg));
		ROIs = Utilities::getDivisions(1, 1, 1, false, globalROI, true, false);
	}
	virtual void sendImageMainLoop()
	{
		for (int i = 0; i < amplitudes[0].size(); i++)
		{
			Mat frame = img.clone();
			vector<Mat> BGR;
			cv::split(frame, BGR);
			// blue is poitive
			Utilities::updateFrameLuminance(BGR[0], ROIs[0], amplitudes[0][i]);
			// red is negative
			Utilities::updateFrameLuminance(BGR[2], ROIs[0], -amplitudes[0][i]);
			// green is control
			// not yet implemented
			cv::merge(BGR, frame);
			vidWriter << frame;
		}
	}
	virtual void sendVideoMainLoop()
	{
		Mat frame;
		double frameIndex = 0;
		for (int k = 0; k < amplitudes[0].size(); k++)
		{
			if (k >= frameIndex)
			{
				frameIndex += inputFrameUsageFrames;
				videoReader.read(frame);
			}
			Mat tmp;
			cv::resize(frame, tmp, Utilities::getFrameSize());
			vector<Mat> BGR;
			cv::split(tmp, BGR);
			// blue is poitive
			Utilities::updateFrameLuminance(BGR[0], ROIs[0], amplitudes[0][k]);
			// red is negative
			Utilities::updateFrameLuminance(BGR[2], ROIs[0], -amplitudes[0][k]);
			// green is control
			// not yet implemented
			cv::merge(BGR, tmp);
			vidWriter << tmp;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////
	///              //////////////      Receive     ///////////////                         ////
	/////////////////////////////////////////////////////////////////////////////////////////////
	virtual vector<Mat> getSplittedImages(Mat &frame)
	{
		vector<Mat> ret;
		cv::split(frame, ret);
		return ret;
	}

	// receive with a certain ROI ratio
	vector<short> receive(string fileName, double ROI_Ratio)
	{
		Parameters::BKGMaskThr = 300;
		Parameters::CommunicatorSpecificSplit = 1;
		int fps = 0;
		vector<vector<float> > frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps, 1, 1, true, false);
		vector<float> frames_BGR;
		for (int i = 0; i < frames[0].size(); i+=3)
		{
			frames_BGR.push_back(frames[0][i] - frames[0][i + 2]);
		}

		return receive2(frames_BGR, fps);
	}
};

