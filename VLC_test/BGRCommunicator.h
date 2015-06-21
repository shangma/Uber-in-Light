/*
Copyright (c) 2015, mostafa izz
izz.mostafa@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the name of MyVLC nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once
#include "SplitScreenCommunicator.h"
class BGRCommunicator :
	public SplitScreenCommunicator
{
public:
	BGRCommunicator()
	{
		Parameters::BKGMaskThr = 300;
		Parameters::amplitudeExtraction = BGR_CHANNELS;
	}
	////////////////////////////// Split Amplitude ///////////////////////////
	virtual string getVideoName(string outputVideoFile)
	{
		return "_RGB_" + outputVideoFile;
	}
	virtual void initCommunication()
	{
		// divide the message into 3 parts
		vector<SymbolData> DivMsg[3];
		for (int i = 0; i < msg.size(); i++)
		{
			DivMsg[i % 3].push_back(msg[i]);
		}
		for (int i = 1; i < 3; i++)
		{
			if (DivMsg[i].size() < DivMsg[0].size())
			{
				DivMsg[i].push_back(DivMsg[0][0]);
			}
		}
		for (int i = 0; i < 3; i++)
		{
			amplitudes.push_back(WaveGenerator::createWaveGivenFPS(DivMsg[i]));
		}
		framesForSymbol = (Parameters::fps * Parameters::symbolTime) / 1000;

		ROIs = Utilities::getDivisions(Parameters::sideA, Parameters::sideB, 1, false, Parameters::globalROI, true, 1,1);
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
				//vector<Mat> BGR;
				//cv::split(img, BGR);
				Mat frame(img.clone());
				int innerLoopIndex = i + k;
				for (int j = 0; j < sections && innerLoopIndex < amplitudes0_size; j++, innerLoopIndex += framesForSymbol)
				{
					// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
					double tmpAmplitudes[3];
					for (int k = 0; k < 3; k++)
					{
						//Utilities::updateFrameWithVchannel(BGR[k], ROIs[j], amplitudes[k][innerLoopIndex]);
						tmpAmplitudes[k] = amplitudes[k][innerLoopIndex];
					}
					Utilities::updateFrameWithVchannel(frame, ROIs[j], tmpAmplitudes);
				}
				//Mat frame;
				//cv::merge(BGR, frame);
				writeFrame(frame);
			}
		}
	}
	virtual void sendVideoMainLoop()
	{
		//double frameIndexIncrement = inputFrameUsageFrames*sections;
		int ampitudesSize = amplitudes[0].size();
		int i_increment = (sections * framesForSymbol);
		for (int i = 0; i < ampitudesSize; i += i_increment)
		{
			for (int k = 0; k < framesForSymbol; k++, frameIndexComparison++)
			{
				if (frameIndexComparison >= frameIndex)
				{
					frameIndex += inputFrameUsageFrames;
					videoReader.read(img);
					cv::resize(img, img, Utilities::getFrameSize());
				}
				//vector<Mat> BGR;
				//cv::split(img, BGR);
				Mat frame(img.clone());
				int innerLoopIndex = i + k;
				for (int j = 0; j < sections && innerLoopIndex < ampitudesSize; j++, innerLoopIndex += framesForSymbol)
				{
					double tmpAmplitudes[4];
					// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
					for (int k = 0; k < 3; k++)
					{
						//Utilities::updateFrameWithVchannel(BGR[k], ROIs[j], amplitudes[k][innerLoopIndex]);
						tmpAmplitudes[k] = amplitudes[k][innerLoopIndex];
					}
					Utilities::updateFrameWithVchannel(frame, ROIs[j], tmpAmplitudes);
				}
				//Mat frame;
				//cv::merge(BGR, frame);
				writeFrame(frame);
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
		vector<vector<float> > frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, Parameters::fps,
			Parameters::sideA, Parameters::sideB, true, 1,1);
		vector<vector<float> > frames_BGR;
		for (int i = 0; i < frames.size(); i++)
		{
			vector<float> B,G,R;
			for (int j = 0; j < frames[i].size(); j += 3)
			{
				// blue
				B.push_back(frames[i][j]);
				// gree
				G.push_back(frames[i][j + 1]);
				// red
				R.push_back(frames[i][j + 2]);
			}
			frames_BGR.push_back(B);
			frames_BGR.push_back(G);
			frames_BGR.push_back(R);
		}
		int frames_per_symbol = Parameters::fps * Parameters::symbolTime / 1000;

		return receiveN(frames_BGR, Parameters::fps, frames_per_symbol);
	}
};

