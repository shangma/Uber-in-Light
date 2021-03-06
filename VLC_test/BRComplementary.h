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

/*
Do you mean brightness? Perceived brightness? Luminance?

Luminance (standard for certain colour spaces): (0.2126*R + 0.7152*G + 0.0722*B) [1]
Luminance (perceived option 1): (0.299*R + 0.587*G + 0.114*B) [2]
Luminance (perceived option 2, slower to calculate): sqrt( 0.299*R^2 + 0.587*G^2 + 0.114*B^2 )
*/
class BRComplementary :
	public BGRCommunicator
{
public:
	////////////////////////////// Split Amplitude ///////////////////////////
	virtual string getVideoName(string outputVideoFile)
	{
		return "_RGB2_" + outputVideoFile;
	}
	virtual void initCommunication()
	{
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(msg, Parameters::fps, Parameters::symbolTime,1));
		framesForSymbol = (Parameters::fps * Parameters::symbolTime) / 1000;
		amplitudes.push_back(vector<float>());
		amplitudes.push_back(vector<float>());
		for (int i = 0; i < amplitudes[0].size(); i++)
		{
			amplitudes[1].push_back(0);
			amplitudes[2].push_back(-amplitudes[0][i]);
		}
		ROIs = Utilities::getDivisions(Parameters::sideA, Parameters::sideB, 1, false, Parameters::globalROI, true, 1,1);
		sections = Parameters::getNumberofGridCells();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	///              //////////////      Receive     ///////////////                         ////
	/////////////////////////////////////////////////////////////////////////////////////////////
	vector<short> receive(string fileName, double ROI_Ratio)
	{
		Parameters::BKGMaskThr = 300;
		vector<vector<float> > frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, Parameters::fps, 
			Parameters::sideA, Parameters::sideB, true, 1,1);
		vector<vector<float> > BRDiff;
		for (int i = 0; i < frames.size(); i++)
		{
			vector<float> temp;
			for (int j = 0; j < frames[i].size(); j += 3)
			{
				temp.push_back(frames[i][j] - frames[i][j + 2]);
			}
			BRDiff.push_back(temp);
		}
		int frames_per_symbol = Parameters::fps * Parameters::symbolTime / 1000;
		if (Parameters::synchMethod == SYNCH_GREEN_CHANNEL || Parameters::synchMethod == SYNCH_COMBINED)
		{
			return receiveNCombined(BRDiff, Parameters::fps, frames_per_symbol);
		}
		return receiveN(BRDiff, Parameters::fps, frames_per_symbol);
	}
};

