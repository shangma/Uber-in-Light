#pragma once
#include "Communicator.h"

class SplitScreenCommunicator :
	public Communicator
{
public:
	////////////////////////////// Split to quarters ///////////////////////////
	int sections;
	
	int framesForSymbol;
	virtual string getVideoName(string outputVideoFile)
	{
		ostringstream ostr;
		ostr << "_Split_" << outputVideoFile;
		return ostr.str();
	}

	virtual void initCommunication()
	{
		//double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(msg));
		
		framesForSymbol = (Parameters::fps * Parameters::symbolTime) / 1000;
		
		ROIs = Utilities::getDivisions(Parameters::sideA,Parameters::sideB, 1, false, globalROI, true,false);
		sections = Parameters::sideA * Parameters::sideB;
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
					Utilities::updateFrameLuminance(frame, ROIs[j], amplitudes[0][i + (j * framesForSymbol) + k]);
				}
				vidWriter << frame;
			}
		}
	}
	virtual void sendVideoMainLoop()
	{
		double frameIndex = 0;
		double frameIndexIncrement = inputFrameUsageFrames*sections;
		int ampitudesSize = amplitudes[0].size();
		int i_increment = (sections * framesForSymbol);
		for (int i = 0; i < ampitudesSize; i += i_increment)
		{
			int frameIndexComparison = i;
			for (int k = 0; k < framesForSymbol; k++, frameIndexComparison += sections)
			{
				if (frameIndexComparison >= frameIndex)
				{
					frameIndex += frameIndexIncrement;
					videoReader.read(img);
					cv::resize(img, img, Utilities::getFrameSize());
				}
				Mat frame = img.clone();
				int innerLoopComparison = i + k;
				for (int j = 0; j < sections && innerLoopComparison < ampitudesSize; j++, innerLoopComparison += framesForSymbol)
				{
					// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
					Utilities::updateFrameLuminance(frame, ROIs[j], amplitudes[0][innerLoopComparison]);
				}
				vidWriter << frame;
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	///              //////////////      Receive     ///////////////                         ////
	/////////////////////////////////////////////////////////////////////////////////////////////
	vector<short> receiveN(vector<vector<float> > frames, int fps)
	{
		int frames_per_symbol = fps * Parameters::symbolTime / 1000;
		sections = Parameters::sideA * Parameters::sideB;
		vector<short> result;
		if (frames.size() == 0)
			return result;
		vector<vector<short>> vt;
		for (int k = 0; k < sections; k++)
		{
			vt.push_back(receive2(frames[k], fps));
		}
		int symbolSize = Parameters::symbolsData.allData[0].symbol.size();
		for (int i = 0; i < vt[0].size(); i += symbolSize)
		{
			for (int j = 0; j < vt.size(); j++)
			{
				for (int k = 0; k < symbolSize; k++)
				{
					result.push_back(vt[j][i + k]);
				}
			}
		}
		return result;
	}

	// receive with a certain ROI ratio
	vector<short> receive(string fileName, double ROI_Ratio)
	{
		//Parameters::BKGMaskThr = 300;
		int fps = 0;
		vector<vector<float> > frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps, Parameters::sideA, Parameters::sideB, true, false);
		return receiveN(frames, fps);
	}
};

