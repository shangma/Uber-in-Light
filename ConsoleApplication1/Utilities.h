#pragma once

#include "Header.h"
// 0 means 20 hz and 1 is 30 hz
const double FREQ[] = { 10, 6 };
const int LUMINANCE[] = { 1, -1 };
enum{ ZERO = 0, ONE };
const double EPSILON = 1e-10;
const double M_PI = 3.14159265359;

// the frequency component and the percentage it represent in the frequency components given
struct Frequency
{
	float freq;
	float percent;
};

class Utilities
{
public:
	/// This function will not check the range for the ROI
	/// this function adds alpha to the value channel in the selected ROI
	static void updateFrameWithAlpha(Mat &frame, Rect ROI, double alpha)
	{
		Mat tmp;
		cv::cvtColor(frame, tmp, CV_BGR2HSV);
		vector<Mat> HSV(3);
		split(tmp, HSV);
		//HSV[2] = 0 * HSV[2] + (alpha / 100.0) * 255;
		Mat aux = HSV[2](ROI);
		aux = (aux + alpha);
		merge(HSV, tmp);
		cv::cvtColor(tmp, frame, CV_HSV2BGR);
	}
	static int gcd(int a, int b)
	{
		if (b == 0) return a;
		return gcd(b, a%b);
	}

	static int lcm(int a, int b)
	{
		return (a * (b / gcd(a, b)));
	}

	static void myft()
	{
		double Fs = 30;                    // Sampling frequency
		double T = 1 / Fs;                     // Sample time
		int L = 24;                     // Length of signal
		// Sum of a 50 Hz sinusoid and a 120 Hz sinusoid
		// t = (0:L - 1)*T;                % Time vector
		Mat inp = Mat::zeros(1, L, CV_32F);
		for (int i = 0; i < L; i++)
		{
			((float*)inp.data)[i] = sin(2 * M_PI * 14 * (i*T));// +sin(2 * M_PI * 30 * (i*T));
		}

		Mat planes[] = { Mat_<float>(inp), Mat::zeros(inp.size(), CV_32F) };
		Mat complexI;
		merge(planes, 2, complexI);         // Add to the expanded another plane with zeros

		Mat oup;
		cv::dft(complexI, oup);

		split(oup, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
		magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
		Mat magI = planes[0];

		for (int i = 0; i < oup.cols / 2; i++){
			//if (((float*)magI.data)[i] > .1)
				{
					cout << i * (Fs / oup.cols) << "\t" << ((float*)magI.data)[i] << endl;
				}
		}
		cout << endl;
	}

	// take array of inputs: luminance
	// sampling frequency: Fs
	// start from frame number: start
	// Number of frames to take: L
	// returns vector<Frequency> 
	static vector<Frequency> myft(vector<float> luminance, double Fs, int start = 0, int L = 30)
	{
		Mat inp = Mat::zeros(1, L, CV_32F);
		for (int i = 0; i < L; i++)
		{
			((float*)inp.data)[i] = luminance[i + start];
		}

		Mat planes[] = { Mat_<float>(inp), Mat::zeros(inp.size(), CV_32F) };
		Mat complexI;
		merge(planes, 2, complexI);         // Add to the expanded another plane with zeros

		Mat oup;
		cv::dft(complexI, oup);

		split(oup, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
		magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
		Mat magI = planes[0];

		vector<Frequency> ret;
		float total = 1;
		for (int i = 1; (i * (Fs / oup.cols)) <= Fs / 2; i++){
			//if (((float*)magI.data)[i] > 10)
			//{
			//cout << i * (Fs / oup.cols) << "\t" << ((float*)magI.data)[i] << endl;
			//}
			Frequency f;
			f.freq = i * (Fs / oup.cols);
			f.percent = ((float*)magI.data)[i];
			total += f.percent;
			ret.push_back(f);
		}
		// cout << endl;
		for (int i = 0; i < ret.size(); i++){
			ret[i].percent /= total;
			// display for testing
			//cout << ret[i].freq << "\t" << ret[i].percent << endl;
		}

		return ret;
	}


};