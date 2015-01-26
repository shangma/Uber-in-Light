#pragma once

#include "Header.h"

struct ReceiveParameters
{
	static int startingIndex;
	static int framesPerSymbol;
	static cv::Rect globalROI;
	static vector<float> amplitudes;
};
int ReceiveParameters::startingIndex = 0;
int ReceiveParameters::framesPerSymbol = 30;
cv::Rect ReceiveParameters::globalROI = cv::Rect(0,0,1,1);
vector<float> ReceiveParameters::amplitudes = vector<float>();