#pragma once
#include "Header.h"

// this class will be generating waves
class WaveGenerator
{
public:
	// symbol_time: how many milliseconds will the symbol last
	static vector<float> createWaveGivenFPS(double frequency, vector<short> msg, int symbol_time, int ZeroFrequency, int OneFrequency, double luminance[2])
	{
		vector<float> amplitudes;
		int framerate = frequency; //get the frame rate
		int frames_per_symbol = (framerate * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
		int frequencies[] = { ZeroFrequency, OneFrequency };
		// create the video writer
		VideoWriter vidWriter;
		for (int i = 0; i < msg.size(); i++)
		{
			int needed_frequency = frequencies[msg[i]];
			int frames_per_half_cycle = framerate / (needed_frequency * 2);
			// start high
			int luminance_index = 0;
			for (int k = 0; k < frames_per_symbol; k++)
			{
				if ((k%frames_per_half_cycle) == 0)
				{
					luminance_index ^= 1;
				}
				//cout << luminance_index;
				amplitudes.push_back(luminance[luminance_index]);
			}
			cout << msg[i];
		}
		cout << endl;
		return amplitudes;
	}

	// create sampled sine wave
	static vector<float> createSampledSineWave(int fps, int frames_per_symbol, float freq)
	{
		vector<float> result;
		for (int i = 0; i < frames_per_symbol; i++)
		{
			result.push_back(sin(2 * MM_PI * freq * i / fps));
		}

		return result;
	}

	// create sampled square wave
	static  vector<float> createSampledSquareWave(int fps, int frames_per_symbol, float freq)
	{
		vector<float> result;
		for (int i = 0; i < frames_per_symbol; i++)
		{
			if (sin(2 * MM_PI * freq * i / fps) > 0)
			{
				result.push_back(1);
			}
			else
			{
				result.push_back(-1);
			}
		}

		return result;
	}
};