#pragma once

#include "SplitScreenCommunicator.h"

class SplitScreenAmpDifferenceCommunicator :
	public SplitScreenCommunicator
{
public:
	////////////////////////////// Split to quarters ///////////////////////////
	virtual string getVideoName(string outputVideoFile)
	{
		ostringstream ostr;
		ostr << "_SplitAmp_" << outputVideoFile;
		return ostr.str();
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
		
		framesForSymbol = (Parameters::fps * Parameters::symbolTime) / 1000;
		 
		ROIs = Utilities::getDivisions(Parameters::sideA, Parameters::sideB, 1, false, globalROI, true, true);
		sections = Parameters::sideA* Parameters::sideB;
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
					Utilities::updateFrameLuminance(frame, ROIs[j * 2],	amplitudes[0][i + (j * framesForSymbol) + k]);
					Utilities::updateFrameLuminance(frame, ROIs[j * 2 + 1],	amplitudes[1][i + (j * framesForSymbol) + k]);
				}
				vidWriter << frame;
			}
		}
	}
	virtual void sendVideoMainLoop()
	{
		double frameIndex = 0;
		for (int i = 0; i < amplitudes[0].size(); i += (sections * framesForSymbol))
		{
			for (int k = 0; k < framesForSymbol; k++)
			{
				if ((i + k * sections) >= frameIndex)
				{
					frameIndex += inputFrameUsageFrames * sections;
					videoReader.read(img);
					cv::resize(img, img, Utilities::getFrameSize());
				}
				Mat frame = img.clone();
				for (int j = 0; j < sections && (i + (j * framesForSymbol) + k) < amplitudes[0].size(); j++)
				{
					// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
					Utilities::updateFrameLuminance(frame, ROIs[j * 2],	amplitudes[0][i + (j * framesForSymbol) + k]);
					Utilities::updateFrameLuminance(frame, ROIs[j * 2 + 1],	amplitudes[1][i + (j * framesForSymbol) + k]);
				}
				vidWriter << frame;
			}
		}
	}

	// receive with a certain ROI ratio
	vector<short> receive(string fileName, double ROI_Ratio)
	{
		int fps = 0;
		vector<vector<float> > frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps, Parameters::sideA, Parameters::sideB, true, true);
		vector<vector<float> > amplitude_difference;
		for (int j = 0; j < frames.size(); j += 2)
		{
			vector<float> tmp;
			for (int i = 0; i < frames[j].size(); i++)
			{
				tmp.push_back(frames[j][i] - frames[j + 1][i]);
			}
			amplitude_difference.push_back(tmp);
		}
		return receiveN(amplitude_difference, fps);
	}
};

