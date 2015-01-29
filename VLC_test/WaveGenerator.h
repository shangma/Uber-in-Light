#pragma once
#include "Header.h"

// this class will be generating waves
class WaveGenerator
{
public:
	// symbol_time: how many milliseconds will the symbol last
	static vector<float> createWaveGivenFPS(double frequency, vector<short> &msg, int symbol_time, int ZeroFrequency, int OneFrequency, double luminance[2],
		bool sampling = true)
	{
		if (sampling)
		{
			return createWaveBySampling(frequency,msg,symbol_time,ZeroFrequency,OneFrequency,luminance);
		}
		return createWaveAssumingIntegers(frequency, msg, symbol_time, ZeroFrequency, OneFrequency, luminance);
	}
	static vector<float> createWaveAssumingIntegers(double frequency, vector<short> &msg, int symbol_time, int ZeroFrequency, int OneFrequency, double luminance[2])
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
	static vector<float> createWaveBySampling(double frequency, vector<short> &msg, int symbol_time, int ZeroFrequency, int OneFrequency, double luminance[2])
	{
		vector<float> amplitudes;
		int framerate = frequency; //get the frame rate
		int frames_per_symbol = (framerate * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
		//int frequencies[] = { ZeroFrequency, OneFrequency };
		// create the video writer
		vector<float> sample[2];
		sample[0] = WaveGenerator::createSampledSquareWave(frequency, frames_per_symbol, ZeroFrequency, luminance[0], luminance[1]);
		sample[1] = WaveGenerator::createSampledSquareWave(frequency, frames_per_symbol, OneFrequency, luminance[0], luminance[1]);
		VideoWriter vidWriter;
		for (int i = 0; i < msg.size(); i++)
		{
			amplitudes.insert(amplitudes.end(), sample[msg[i]].begin(), sample[msg[i]].end());
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
			result.push_back(sin(2 * MM_PI * freq * i / fps + MM_PI));
		}

		return result;
	}

	// create sampled square wave
	static vector<float> createSampledSquareWave(int fps, int frames_per_symbol, float freq, float pos, float neg)
	{
		vector<float> result;
		for (int i = 0; i < frames_per_symbol; i++)
		{
			float val = sin(2 * MM_PI * freq * i / fps + MM_PI);
			if (val > 0)
			{
				result.push_back(pos);
			}
			else if (val < 0)
			{
				result.push_back(neg);
			}
			else
			{
				result.push_back(0);
			}
		}

		return result;
	}
};