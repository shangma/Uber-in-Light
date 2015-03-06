#pragma once
#include "Communicator.h"
class BGRCommunicator3 :
	public Communicator
{
public:
	////////////////////////////// Split Amplitude ///////////////////////////
	virtual string getVideoName(string outputVideoFile)
	{
		return "_RGB3_" + outputVideoFile;
	}
	virtual void initCommunication()
	{
		// divide the message into 2 parts
		vector<SymbolData> DivMsg[2];
		for (int i = 0; i < msg.size(); i++)
		{
			DivMsg[i % 2].push_back(msg[i]);
		}
		for (int i = 1; i < 2; i++)
		{
			if (DivMsg[i].size() < DivMsg[0].size())
			{
				DivMsg[i].push_back(DivMsg[0][0]);
			}
		}
		// blue
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(DivMsg[0]));
		// green
		amplitudes.push_back(vector<float>());
		//red
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(DivMsg[1]));
		
		// then add the green channel as the inverse of the other two channels
		for (int i = 0; i < amplitudes[0].size(); i++)
		{
			amplitudes[1].push_back(-amplitudes[0][i] - amplitudes[2][i]);
		}
		ROIs = Utilities::getDivisions(1, 1, 1, false, globalROI, true, false);
	}
	virtual void sendImageMainLoop()
	{
		for (int i = 0; i < amplitudes[0].size(); i++)
		{
			Mat frame = img.clone();
			vector<Mat> BGR;
			cv::split(frame, BGR);
			for (int j = 0; j < 3; j++)
			{
				Utilities::updateFrameLuminance(BGR[j], ROIs[0], amplitudes[j][i]);
			}
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
			for (int j = 0; j < 3; j++)
			{
				Utilities::updateFrameLuminance(BGR[j], ROIs[0], amplitudes[j][k]);
			}
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
		Parameters::BKGMaskThr = 10;
		Parameters::CommunicatorSpecificSplit = 1;
		int fps = 0;
		vector<vector<float> > frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps, 1, 1, true, false);
		vector<float> frames_BGR[2];
		for (int i = 0; i < frames[0].size(); i+=3)
		{
			// blue
			frames_BGR[0].push_back(frames[0][i]);
			// red
			frames_BGR[1].push_back(frames[0][i + 2]);
		}
		vector<short> tmp_res[2];
		for (int i = 0; i < 2; i++)
		{
			tmp_res[i] = receive2(frames_BGR[i], fps);
		}
		vector<short> res;
		for (int i = 0; i < tmp_res[0].size(); i++)
		{
			for (int j = 0; j < 2; j++)
			{
				res.push_back(tmp_res[j][i]);
			}
		}
		return res;
	}
};