#pragma once

#include "Header.h"

struct Parameters
{
	static int startingIndex;
	static int framesPerSymbol;
	static cv::Rect globalROI;
	static vector<float> amplitudes;
	static int fps;
};
int Parameters::startingIndex = 0;
int Parameters::framesPerSymbol = 30;
cv::Rect Parameters::globalROI = cv::Rect(0,0,1,1);
vector<float> Parameters::amplitudes = vector<float>();
int Parameters::fps = 0;