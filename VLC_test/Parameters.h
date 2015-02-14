#pragma once

#include "Header.h"

enum
{
	FFT_RANDOM_GUESS,
	FFT_NO_RANDOM_GUESS,
	CROSS_CORRELATION,
};

struct Parameters
{
	static int startingIndex;
	static int framesPerSymbol;
	static cv::Rect globalROI;
	static vector<float> amplitudes;
	static int fps;
	static int DecodingMethod;
};
int Parameters::startingIndex = 0;
int Parameters::framesPerSymbol = 30;
cv::Rect Parameters::globalROI = cv::Rect(0,0,1,1);
vector<float> Parameters::amplitudes = vector<float>();
int Parameters::fps = 0;
int Parameters::DecodingMethod = FFT_RANDOM_GUESS;