#pragma once
#include "Communicator.h"
class SplitAmplitudeCommunicator :
	public Communicator
{
public:
	////////////////////////////// Split Amplitude ///////////////////////////
	virtual string getVideoName(string outputVideoFile)
	{
		return "_AmpDiff_" + outputVideoFile;
	}
	virtual void initCommunication()
	{
		//double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(msg));
		//double lumin2[] = { LUMINANCE[1], LUMINANCE[0] };
		for (int i = 0; i < msg.size(); i++)
		{
			msg[i].amplitude = -msg[i].amplitude;
		}
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(msg));

		ROIs = Utilities::getDivisions(1, 1, 1, false, Parameters::globalROI, true, true);
	}
	virtual void sendImageMainLoop()
	{
		for (int i = 0; i < amplitudes[0].size(); i++)
		{
			Mat frame = img.clone();
			Utilities::updateFrameLuminance(frame, ROIs[0], amplitudes[0][i]);
			Utilities::updateFrameLuminance(frame, ROIs[1], amplitudes[1][i]);
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
			Utilities::updateFrameLuminance(tmp, ROIs[0], amplitudes[0][k]);
			Utilities::updateFrameLuminance(tmp, ROIs[1], amplitudes[1][k]);
			vidWriter << tmp;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////
	///              //////////////      Receive     ///////////////                         ////
	/////////////////////////////////////////////////////////////////////////////////////////////


	// receive with a certain ROI ratio
	vector<short> receive(string fileName, double ROI_Ratio)
	{
		int fps = 0;
		vector<vector<float> > frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps, 1,1,true,true);
		vector<float> amplitude_difference;
		//double avg = 0;
		for (int i = 0; i < frames[0].size(); i++)
		{
			amplitude_difference.push_back(frames[0][i] - frames[1][i]);
			//avg = std::max((double)abs(amplitude_difference[i]),avg);
		}
		//cout << "Avg amplitude = " << avg << endl;
		return receive2(amplitude_difference, fps);
	}
};

