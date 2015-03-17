#pragma once
#include "Communicator.h"
class BGRCommunicator3 :
	public SplitScreenCommunicator
{
public:
	////////////////////////////// Red and Blue as separate channels ///////////////////////////
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
			amplitudes[1].push_back(0);// -amplitudes[0][i] - amplitudes[2][i]);
		}
		framesForSymbol = (Parameters::fps * Parameters::symbolTime) / 1000;

		ROIs = Utilities::getDivisions(Parameters::sideA, Parameters::sideB, 1, false, Parameters::globalROI, true, false);
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
					for (int k = 0; k < 3; k++)
					{
						Utilities::updateFrameLuminance(BGR[k], ROIs[j], amplitudes[k][innerLoopIndex]);
					}
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
					for (int k = 0; k < 3; k++)
					{
						Utilities::updateFrameLuminance(BGR[k], ROIs[j], amplitudes[k][innerLoopIndex]);
					}
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
		Parameters::amplitudeExtraction = BR_CHANNELS_SEPARATE;
		//Parameters::CommunicatorSpecificSplit = 1;
		int fps = 0;
		vector<vector<float> > frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps,
			Parameters::sideA, Parameters::sideB, true, false);
		vector<vector<float> > frames_BGR;
		for (int i = 0; i < frames.size(); i++)
		{
			vector<float> B;
			vector<float> R;
			for (int j = 0; j < frames[i].size(); j+=2)
			{
				// blue
				B.push_back(frames[i][j]);
				// red
				R.push_back(frames[i][j + 1]);
			}
			frames_BGR.push_back(B);
			frames_BGR.push_back(R);
		}
		return receiveN(frames_BGR, fps);
	}
};