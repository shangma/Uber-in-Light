#pragma once
#include "Communicator.h"
class OldCommunicator :
	public Communicator
{
public:

	/////////////////////////////////////////////////////////////////////////////////////////////
	///              //////////////      Receive     ///////////////                         ////
	/////////////////////////////////////////////////////////////////////////////////////////////


	// receive with a certain ROI ratio
	vector<short> receive(string fileName, int frames_per_symbol, double ROI_Ratio)
	{
		int fps = 0;
		vector<float> frames = Utilities::getVideoFrameLuminancesOLd(fileName, fps, true, true);
		
		return receive2(frames, 30, frames_per_symbol);
	}
};

