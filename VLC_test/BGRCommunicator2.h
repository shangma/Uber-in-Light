#pragma once
#include "SplitScreenCommunicator.h"

/*
Do you mean brightness? Perceived brightness? Luminance?

Luminance (standard for certain colour spaces): (0.2126*R + 0.7152*G + 0.0722*B) [1]
Luminance (perceived option 1): (0.299*R + 0.587*G + 0.114*B) [2]
Luminance (perceived option 2, slower to calculate): sqrt( 0.299*R^2 + 0.587*G^2 + 0.114*B^2 )
*/
class BGRCommunicator2 :
	public SplitScreenCommunicator
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
		framesForSymbol = (Parameters::fps * Parameters::symbolTime) / 1000;

		ROIs = Utilities::getDivisions(Parameters::sideA, Parameters::sideB, 1, false, globalROI, true, false);
		sections = Parameters::sideA * Parameters::sideB;
	}

	virtual void sendImageMainLoop()
	{
		int amplitudes0_size = amplitudes[0].size();
		int i_increment = (sections * framesForSymbol);
		for (int i = 0; i < amplitudes0_size; i += i_increment)
		{
			for (int k = 0; k < framesForSymbol; k++)
			{
				vector<Mat> BGR;
				cv::split(img, BGR);
				int innerLoopIndex = i + k;
				for (int j = 0; j < sections && innerLoopIndex < amplitudes0_size; j++, innerLoopIndex += framesForSymbol)
				{
					// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
					
					// blue is poitive
					Utilities::updateFrameLuminance(BGR[0], ROIs[j], amplitudes[0][innerLoopIndex]);
					// red is negative
					Utilities::updateFrameLuminance(BGR[2], ROIs[j], amplitudes[0][innerLoopIndex]);
					// green is control
					// not yet implemented
				}
				Mat frame;
				cv::merge(BGR, frame);
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
				vector<Mat> BGR;
				cv::split(img, BGR);

				int innerLoopIndex = i + k;
				for (int j = 0; j < sections && innerLoopIndex < ampitudesSize; j++, innerLoopIndex += framesForSymbol)
				{
					// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
					// blue is poitive
					Utilities::updateFrameLuminance(BGR[0], ROIs[j], amplitudes[0][innerLoopIndex]);
					// red is negative
					Utilities::updateFrameLuminance(BGR[2], ROIs[j], -amplitudes[0][innerLoopIndex]);
					// green is control
					// not yet implemented
				}
				Mat frame;
				cv::merge(BGR, frame);
				vidWriter << frame;
			}
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
		Parameters::amplitudeExtraction = BR_CHANNELS_DIFF;
		int fps = 0;
		vector<vector<float> > frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps, 
			Parameters::sideA, Parameters::sideB, true, false);
		vector<vector<float> > frames_BGR;
	
		for (int j = 0; j < frames.size(); j ++)
		{
			vector<float> tmp;
			for (int i = 0; i < frames[j].size();i++)
			{
				tmp.push_back(frames[j][i]);
			}
			frames_BGR.push_back(tmp);
		}
		return receiveN(frames_BGR, fps);
	}
};

