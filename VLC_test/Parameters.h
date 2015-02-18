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
	static float symbolTime;
	static cv::Rect globalROI;
	static vector<float> amplitudes;
	static int fps;
	static int DecodingMethod;
	// 0 means 20 hz and 1 is 30 hz
	static double FREQ[2];
	static double LUMINANCE;
	//enum{ ZERO = 0, ONE };
	static string codec; //I420, DIB ,DIVX, XVID
	static cv::Size DefaultFrameSize;
	static Size patternsize;
};
int Parameters::startingIndex = 0;
cv::Rect Parameters::globalROI = cv::Rect(0,0,1,1);
vector<float> Parameters::amplitudes = vector<float>();
int Parameters::fps = 0;
float Parameters::symbolTime = 1000;
int Parameters::DecodingMethod = FFT_RANDOM_GUESS;
double Parameters::LUMINANCE = 0.005;
//enum{ ZERO = 0, ONE };
string Parameters::codec = "I420"; //I420, DIB ,DIVX, XVID
cv::Size Parameters::DefaultFrameSize = cv::Size(640, 480);
Size Parameters::patternsize = cv::Size(11, 11);
double Parameters::FREQ[] = { 12, 8 };