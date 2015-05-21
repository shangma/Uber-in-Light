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
#include "BGRCommunicator.h"
class RGBSpatialRedundancyCommunicator :
	public BGRCommunicator
{
public:
	////////////////////////////// Split Amplitude ///////////////////////////
	virtual string getVideoName(string outputVideoFile)
	{
		return "_RGBSpatialRedundancy_" + outputVideoFile;
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
		ROIs = Utilities::getDivisions(1, 1, 1, false, Parameters::globalROI, true, true);
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
				Utilities::updateFrameWithVchannel(BGR[j], ROIs[0], amplitudes[j][i]);
				Utilities::updateFrameWithVchannel(BGR[j], ROIs[1], -amplitudes[j][i]);
			}
			cv::merge(BGR, frame);
			writeFrame(frame);
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
				Utilities::updateFrameWithVchannel(BGR[j], ROIs[0], amplitudes[j][k]);
				Utilities::updateFrameWithVchannel(BGR[j], ROIs[1], -amplitudes[j][k]);
			}
			cv::merge(BGR, tmp);
			writeFrame(tmp);
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////
	///              //////////////      Receive     ///////////////                         ////
	/////////////////////////////////////////////////////////////////////////////////////////////


	// receive with a certain ROI ratio
	vector<short> receive(string fileName, double ROI_Ratio)
	{
		Parameters::BKGMaskThr = 10;
		Parameters::CommunicatorSpecificSplit = 1;
		int fps = 0;
		vector<vector<float> > frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps, 1,1, true, true);
		vector<float> frames_BGR[3];
		for (int i = 0; i < frames[0].size(); i++)
		{
			frames_BGR[i % 3].push_back(frames[0][i] - frames[1][i]);
		}
		vector<short> tmp_res[3];
		for (int i = 0; i < 3; i++)
		{
			tmp_res[i] = receive2(frames_BGR[i], fps);
		}
		vector<short> res;
		for (int i = 0; i < tmp_res[0].size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				res.push_back(tmp_res[j][i]);
			}
		}
		return res;
	}
};

