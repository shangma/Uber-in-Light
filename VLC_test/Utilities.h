#pragma once

//#include "Properties.h"
#include "Header.h"
#include "SelectByMouse.h"
#include "background_subtractor.h"
#include "Parameters.h"
//#include "Properties.h"

//extern class Properties;

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
	// the mask must be same size as the ROI and the colors must be 0 and 1 means use this pixel
	static void updateFrameLuminance(Mat &frame, Rect &ROI, double percentage, bool useAlpha = false)
	{
		if (useAlpha)
		{
			updateFrameWithAlpha(frame, ROI, percentage);
		}
		else
		{
			updateFrameWithVchannel(frame, ROI, percentage);
		}
	}
	static Mat createVLayer(int &width, int &height, double &percentage)
	{
		//Mat aux = frame(ROI);
		//aux = (aux + percentage * 255);
		double val = percentage * 255;
		double absVal = abs(val);
		int addVal = floor(absVal);
		int sign = percentage > 0 ? 1 : -1;
		double diff = absVal - addVal;
		addVal *= sign;
		int sz = (width * height);
		int threshold = diff * sz;
		//cout << threshold << endl;
		// make sure there is no overflow or underflow
		//int tmpBGR[3];
		//std::random_device rd;
		//std::mt19937 mt(rd());
		std::mt19937 mt(19937);
		std::uniform_int_distribution<int> dist(0, sz - 1);
		// create v-layer
		Mat vChannel = Mat::zeros(height, width, CV_32SC1);
		int* vData = (int*)vChannel.data;
		for (int i = 0; i < sz; i++)
		{
			vData[i] = addVal;
			if (dist(mt) < threshold)
			{
				vData[i] += sign;
			}
			/*else
			{
				cout << i << endl;
			}*/
		}
		return vChannel;
	}
	static void updateFrameWithVchannel(Mat &frame, Rect &ROI, double percentage)
	{
		long long a = (percentage * 10000);
		long long b = frame.cols;
		long long c = frame.rows;
		long long key = (a << 30) | (b << 15) | (c);
		map<long long, Mat>::iterator vLayerIterator = Parameters::vLayers.find(key);
		if (vLayerIterator == Parameters::vLayers.end())
		{
			Parameters::vLayers[key] = createVLayer(ROI.width, ROI.height, percentage);
		}
		// and copy
		int* vData = (int*)Parameters::vLayers[key].data;
		//unsigned char* maskData = (unsigned char*)mask.data;
		unsigned char* data = (unsigned char*)frame.data;
		int rows = ROI.y + ROI.height;
		int cols = ROI.x + ROI.width;
		int inc = frame.cols - ROI.width;
		int channels = frame.channels();
		for (int r = ROI.y, i = ROI.y * frame.cols + ROI.x, j = 0; r < rows; r++)
		{
			for (int c = ROI.x; c < cols; c++, j++, i++)
			{
				// copy back
				for (int k = 0; k < channels; k++)
				{
					int ind = i * channels + k;
					//int tmp = (vData[j] * maskData[i + k] + data[ind]);
					int tmp = (vData[j] + data[ind]);
					data[ind] = tmp > 255 ? 255 : (tmp < 0) ? 0 : tmp;
				}
			}
			i += inc;
		}
	}
	static void updateFrameWithAlpha(Mat &frame, Rect ROI, double percentage)
	{
		Mat aux = frame(ROI);
		Mat tmp = aux.clone() * 0;
		
		if (percentage < 0)
		{
			percentage = 0.99;
		}
		else
		{
			percentage = 1;
		}
		double beta = 1 - percentage;
		addWeighted(aux, percentage, tmp, beta, 0.0, aux);
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

	static int getOuputVideoFrameRate(int inputFps)
	{
		//if (Parameters::fps)
		//{
			return Parameters::fps;
		//}
		//return lcm(inputFps, Utilities::lcm(2 * FREQ[ZERO], 2 * FREQ[ONE]));
	}
	/*
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
			((float*)inp.data)[i] = sin(2 * MM_PI * 14 * (i*T));// +sin(2 * M_PI * 30 * (i*T));
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
	}*/

	// take array of inputs: luminance
	// sampling frequency: Fs
	// start from frame number: start
	// Number of frames to take: L
	// returns vector<Frequency> 
	static vector<Frequency> myft(vector<float> luminance, double Fs, int start, int L)
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

	// x0,y0 are from the prev frame
	// x1,y1 are from the current frame
	// frames here are V-channels onlt in floating point
	static float getDifference(Mat &prev, Mat &frame, int x0, int y0,int x1,int y1)
	{
		if (x1 < 0 || x1 >= frame.cols)
		{
			return 10000000000;
		}
		if (y1 < 0 || y1 >= frame.rows)
		{
			return 10000000000;
		}
		// then get the real difference
		return (((float*)frame.data)[y1 * frame.cols + x1] - ((float*)prev.data)[y0 * frame.cols + x0]);
	}

	// the input frames here are the V-channels only in floating point
	static float getSmallestDifference(Mat &prev, Mat &frame, int radius, int x, int y)
	{
		// loop on all points in HSV1[2] and check the nearest point corresponds to this point
		float minimum = getDifference(prev, frame, x, y, x, y);
		// check the excact location
		for (int r = 1; r <= radius; r++)
		{
			for (int i = 0; i <= r; i++)
			{
				int j = r - i;
				if (i == 0)
				{
					// check i , j
					minimum = std::min(minimum, getDifference(prev, frame, x, y, x, y + j));
					// check i , -j
					minimum = std::min(minimum, getDifference(prev, frame, x, y, x, y - j));
				}
				else if (j == 0)
				{
					// check i , j
					minimum = std::min(minimum, getDifference(prev, frame, x, y, x + i, y));
					// check -i , j
					minimum = std::min(minimum, getDifference(prev, frame, x, y, x - i, y));
				}
				else
				{
					// check i , j
					minimum = std::min(minimum, getDifference(prev, frame, x, y, x + i, y + j));
					// check i , -j
					minimum = std::min(minimum, getDifference(prev, frame, x, y, x + i, y - j));
					// check -i , j
					minimum = std::min(minimum, getDifference(prev, frame, x, y, x - i, y + j));
					// check -i , -j
					minimum = std::min(minimum, getDifference(prev, frame, x, y, x - i, y - j));
				}
			}
		}
		return minimum;
	}

	static void getVchannel(Mat &frame, vector<float> &amplitudes, Mat &mask)
	{
		vector<Mat> BGR1;
		cv::split(frame, BGR1);
		cv::max(BGR1[0], BGR1[1], BGR1[1]);
		cv::max(BGR1[1], BGR1[2], BGR1[2]);

		float luminance = cv::mean(BGR1[2]).val[0];
		//float summation = (cv::sum((*add_mask)(ROIs[i])).val[0] / 255.0) + 1;
		float summation = (cv::sum(mask).val[0] / 255.0) + 1;
		luminance /= summation;
		if (abs(luminance) < 0.001 && amplitudes.size())
		{
			amplitudes.push_back(amplitudes[amplitudes.size() - 1]);
		}
		else
		{
			amplitudes.push_back(luminance);
		}
	}
	static void getIntensity(Mat &frame, vector<float> &amplitudes, Mat &mask)
	{
		// save the ROI
		Mat tmp = Mat::zeros(frame.size(),CV_32FC1);
		int sz = frame.cols * frame.rows;
		int channels = frame.channels();
		unsigned char * frameData = (unsigned char *)frame.data;
		float* intensityData = (float*)tmp.data;
		for (int i = 0; i < sz; i++)
		{
			for (int j = 0; j < channels; j++)
			{
				intensityData[i] += frameData[i*channels + j];
			}
		}
		float luminance = cv::mean(tmp).val[0];
		//float summation = (cv::sum((*add_mask)(ROIs[i])).val[0] / 255.0) + 1;
		float summation = (cv::sum(mask).val[0] / 255.0) + 1;
		luminance /= summation;
		if (abs(luminance) < 0.001 && amplitudes.size())
		{
			amplitudes.push_back(amplitudes[amplitudes.size() - 1]);
		}
		else
		{
			amplitudes.push_back(luminance);
		}
	}

	// get differnce between neighbour frames
	// and get difference between B and R channels
	static Mat getDiffBetweenFramesBR_G(Mat &prev, Mat &frame, cv::Rect &roi, vector<float> &amplitudes, Mat &mask)
	{
		Mat ret = Mat::zeros(roi.height, roi.width, CV_32F);
		float* data = ((float*)ret.data);
		unsigned char * p = ((unsigned char*)prev.data);
		unsigned char * f = ((unsigned char*)frame.data);
		int sz = ret.cols * ret.rows;
		int rows = roi.y + roi.height;
		int cols = roi.x + roi.width;
		int j = 0;
		int i = roi.y * frame.cols + roi.x;
		int inc = frame.cols - roi.width;
		int channels = frame.channels();
		for (int r = roi.y; r < rows; r++)
		{
			for (int c = roi.x; c < cols; c++, j++, i++)
			{
				int a = f[i * channels + 1] + p[i * channels] + p[i * channels + 2];
				int b = p[i * channels + 1] + f[i * channels] + f[i * channels + 2];
				data[j] = a - b;
			}
			i += inc;
		}

		float luminance = cv::mean(ret).val[0];
		//float summation = (cv::sum((*add_mask)(ROIs[i])).val[0] / 255.0) + 1;
		float summation = (cv::sum(mask).val[0] / 255.0) + 1;
		luminance /= summation;
		if (abs(luminance) < 0.001 && amplitudes.size())
		{
			amplitudes.push_back(amplitudes[amplitudes.size() - 1]);
		}
		else
		{
			amplitudes.push_back(luminance);
		}

		return ret;
	}
	// get differnce between neighbour frames
	// and get difference between B and R channels
	static void getDiffBetweenFramesBR(Mat &prev, Mat &frame, cv::Rect &roi,vector<float> &amplitudes)
	{
		// save the ROI
		Mat tmp, tmp1, tmp2;
		vector<Mat> HSV1, HSV2;

		// new method
		//Mat ret = Mat::zeros(roi.height, roi.width, CV_32SC1);
		//int* data = ((int*)ret.data);
		long long sum = 0;
		unsigned char * p = ((unsigned char*)prev.data);
		unsigned char * f = ((unsigned char*)frame.data);
		int sz = frame.cols * frame.rows;
		int rows = roi.y + roi.height;
		int cols = roi.x + roi.width;
		int j = 0;
		int i = roi.y * frame.cols + roi.x;
		int inc = frame.cols - roi.width;
		int channels = frame.channels();
		for (int r = roi.y; r < rows; r++)
		{
			for (int c = roi.x; c < cols; c++, j++, i++)
			{
				int a = f[i * channels] + p[i * channels + 2];
				int b = f[i * channels + 2] + p[i * channels];
				//data[j] = a - b;
				sum += a - b;
			}
			i += inc;
		}

		amplitudes.push_back(sum);
	}
	// get differnce between neighbour frames
	// and get difference between B and R channels
	static void getDiffBetweenFramesSeprateBR(Mat &prev, Mat &frame, cv::Rect &roi, vector<float> &amplitudes)
	{
		// save the ROI
		Mat tmp, tmp1, tmp2;
		vector<Mat> HSV1, HSV2;

		// new method
		//Mat ret = Mat::zeros(roi.height, roi.width, CV_32SC1);
		//int* data = ((int*)ret.data);
		long long sum1 = 0, sum2 = 0;
		unsigned char * p = ((unsigned char*)prev.data);
		unsigned char * f = ((unsigned char*)frame.data);
		int sz = frame.cols * frame.rows;
		int rows = roi.y + roi.height;
		int cols = roi.x + roi.width;
		int j = 0;
		int i = roi.y * frame.cols + roi.x;
		int inc = frame.cols - roi.width;
		int channels = frame.channels();
		for (int r = roi.y; r < rows; r++)
		{
			for (int c = roi.x; c < cols; c++, j++, i++)
			{
				sum1 += f[i * channels];
				sum1 -= p[i * channels];
				sum2 += f[i * channels + 2];
				sum2 -= p[i * channels + 2];
			}
			i += inc;
		}

		amplitudes.push_back(sum1);
		amplitudes.push_back(sum2);
	}
	// the input frames here are the original frames
	static void getDiffInVchannelHSV(Mat &prev, Mat &frame, cv::Rect &roi, vector<float> &amplitudes, Mat &mask)
	{
		// save the ROI
		Mat tmp, tmp1, tmp2;
		vector<Mat> HSV1, HSV2;	

		// new method
		Mat ret = Mat::zeros(roi.height, roi.width, CV_32F);
		float* data = ((float*)ret.data);
		unsigned char * p = ((unsigned char*)prev.data);
		unsigned char * f = ((unsigned char*)frame.data);
		int sz = ret.cols * ret.rows;
		int rows = roi.y + roi.height;
		int cols = roi.x + roi.width;
		int j = 0;
		int i = roi.y * frame.cols + roi.x;
		int inc = frame.cols - roi.width;
		int channels = frame.channels();
		for (int r = roi.y; r < rows; r++)
		{
			for (int c = roi.x; c < cols; c++,j++,i++)
			{
				// previous
				int max1 = p[i * channels];
				// frame
				int max2 = f[i * channels];
				for (int k = 1; k < channels; k++)
				{
					if (max1 < p[i * channels + k])
					{
						max1 = p[i * channels + k];
					}

					if (max2 < f[i * channels + k])
					{
						max2 = f[i * channels + k];
					}
				}
				data[j] = (max2 - max1);
			}
			i += inc;
		}

		float luminance = cv::mean(ret).val[0];
		//float summation = (cv::sum((*add_mask)(ROIs[i])).val[0] / 255.0) + 1;
		float summation = (cv::sum(mask).val[0] / 255.0) + 1;
		luminance /= summation;
		if (abs(luminance) < 0.001 && amplitudes.size())
		{
			amplitudes.push_back(amplitudes[amplitudes.size() - 1]);
		}
		else
		{
			amplitudes.push_back(luminance);
		}
	}


	// convert video fps to certain given fps
	static void convertVideo(string inputVideo, string outputVideo, double newFPS, double starting_second, double ending_second)
	{
		VideoCapture videoReader(inputVideo);
		if (videoReader.isOpened())
		{
			int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
			int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
			int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
			while (frame_height > 1000)
			{
				frame_width /= 2;
				frame_height /= 2;
			}
			int starting_frame = starting_second * framerate;
			int ending_frame = ending_second * framerate - 1;
			videoReader.set(CV_CAP_PROP_POS_FRAMES, starting_frame); //Set index to last frame

			VideoWriter vidWriter;
			vidWriter.open(outputVideo, CV_FOURCC('D', 'I', 'V', 'X'), newFPS, cv::Size(frame_width, frame_height));
			Mat frame;
			int currrentFrame = starting_frame;
			while (videoReader.read(frame) && currrentFrame++ < ending_frame)
			{
				Mat tmp;
				cv::resize(frame, tmp, cv::Size(frame_width, frame_height));
				vidWriter << tmp;
			}
		}
	}
	// convert video fps to certain given fps
	static void repeatVideo(string inputVideo, string outputVideo, double fps, int numberOfRepetitions, double starting_second, double ending_second)
	{
		VideoCapture videoReader(inputVideo);
		if (videoReader.isOpened())
		{
			int numberOfFrames = videoReader.get(CV_CAP_PROP_FRAME_COUNT); // get frame count
			int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
			int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
			int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
			while (frame_height > 1000)
			{
				frame_width /= 2;
				frame_height /= 2;
			}
			int starting_frame = starting_second * framerate;
			int ending_frame = ending_second * framerate - 1;
			
			videoReader.set(CV_CAP_PROP_POS_FRAMES, starting_frame); //Set index to last frame
			VideoWriter vidWriter;
			vidWriter.open(outputVideo, CV_FOURCC('D', 'I', 'V', 'X'), fps, cv::Size(frame_width, frame_height));
			Mat frame;
			for (int i = 0; i < numberOfRepetitions;i++)
			{
				printf("%d\n", i);
				if (i & 1)
				{
					// move backward
					videoReader.set(CV_CAP_PROP_POS_FRAMES, ending_frame);
					for (int j = ending_frame; j >= starting_frame && videoReader.read(frame); j--)
					{
						videoReader >> frame;
						Mat tmp;
						cv::resize(frame, tmp, cv::Size(frame_width, frame_height));
						vidWriter << tmp;
						videoReader.set(CV_CAP_PROP_POS_FRAMES, j - 1);
					}
				}
				else
				{
					// move forward
					videoReader.set(CV_CAP_PROP_POS_FRAMES, starting_frame);
					for (int j = starting_frame; j <= ending_frame && videoReader.read(frame); j++)
					{
						//printf("%d\n", j);
						Mat tmp;
						cv::resize(frame, tmp, cv::Size(frame_width, frame_height));
						//for (int k = 0; k < 5; k++)
						{
							vidWriter << tmp;
						}
					}
				}
				
			}
		}
	}

	// convert video fps to certain given fps
	static void testVideoBackGround(string inputVideo, string outputVideo, double fps, double starting_second, double ending_second)
	{
		VideoCapture videoReader(inputVideo);
		if (videoReader.isOpened())
		{
			int numberOfFrames = videoReader.get(CV_CAP_PROP_FRAME_COUNT); // get frame count
			int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
			int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
			int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
			while (frame_height > 1000)
			{
				frame_width /= 2;
				frame_height /= 2;
			}
			int starting_frame = starting_second * framerate;
			int ending_frame = ending_second * framerate - 1;

			videoReader.set(CV_CAP_PROP_POS_FRAMES, starting_frame); //Set index to last frame
			VideoWriter vidWriter;
			vidWriter.open(outputVideo, CV_FOURCC('D', 'I', 'V', 'X'), fps, cv::Size(frame_width * 2, frame_height * 2));
			Mat frame;
			videoReader.set(CV_CAP_PROP_POS_FRAMES, starting_frame);
			Mat prev;
			if (videoReader.read(prev))
			{
				cv::resize(prev, prev, cv::Size(frame_width, frame_height));
				for (int j = starting_frame; j <= ending_frame && videoReader.read(frame); j++)
				{
					//printf("%d\n", j);
					Mat tmp;
					cv::resize(frame, tmp, cv::Size(frame_width, frame_height));

					Mat output = Mat::zeros(frame_height * 2, frame_width * 2, prev.type());
					prev.copyTo(output(cv::Rect(0, 0, frame_width, frame_height)));
					cv::putText(output, "Previous Frame", cv::Point(30, 30), FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 0, 0), 2);
					tmp.copyTo(output(cv::Rect(frame_width, 0, frame_width, frame_height)));
					cv::putText(output, "Current Frame", cv::Point(30 + frame_width, 30), FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 0, 0), 2);
					Mat old_mask = MaskFactory::getBackgroundMaskAlgo(prev, tmp);
					cvtColor(old_mask, output(cv::Rect(0, frame_height, frame_width, frame_height)), CV_GRAY2BGR);
					Mat new_mask = MaskFactory::getBackgroundMask(prev, tmp);
					cvtColor(new_mask, output(cv::Rect(frame_width, frame_height, frame_width, frame_height)), CV_GRAY2BGR);
					cv::putText(output, "Old Mask", cv::Point(30, 30 + frame_height), FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 0, 0), 2);
					cv::putText(output, "New Mask", cv::Point(30 + frame_width, 30 + frame_height), FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 0, 0), 2);
					Mat tmp2;
					cv::resize(output, tmp2, getFrameSize());
					//imshow("output", tmp2);
					//cv::waitKey(0);
					vidWriter << output;

					prev = tmp.clone();
				}
			}
		}
	}

	// calculate correlation in video frames
	static void correlateVideoDifference(string inputVideo, double starting_second, double ending_second, int n)
	{
		VideoCapture videoReader(inputVideo);
		if (videoReader.isOpened())
		{
			Mat frame, next;
			int numberOfFrames = videoReader.get(CV_CAP_PROP_FRAME_COUNT); // get frame count
			int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
			int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
			int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
			int starting_frame = starting_second * framerate;
			int ending_frame = ending_second * framerate - 1;
			videoReader.set(CV_CAP_PROP_POS_FRAMES, starting_frame); //Set index to last frame
			vector<double> correlations(n + 1, 0);
			vector<double> correlations_RGB(n + 1, 0);
			vector<int> count(n + 1, 0);
			for (int i = 1; i <= n; i++)
			{
				cout << i << endl;
				videoReader.set(CV_CAP_PROP_POS_FRAMES, starting_frame);
				for (int j = starting_frame; j <= ending_frame && videoReader.read(frame); j++)
				{
					videoReader.set(CV_CAP_PROP_POS_FRAMES, j + i);
					if (videoReader.read(next))
					{
						vector<float> diff;
						Mat mask = Mat::zeros(frame.size(), CV_8UC1) + 255;
						getDiffInVchannelHSV(frame, next, cv::Rect(0, 0, frame_width, frame_height), diff,mask);
						correlations[i] += diff[0];
						
						frame.convertTo(frame, CV_32F);
						vector<Mat> BGR1;
						cv::split(frame, BGR1);
						BGR1[0] = BGR1[0] + BGR1[1];
						BGR1[0] = BGR1[0] + BGR1[2];
						next.convertTo(next, CV_32F);
						vector<Mat> BGR2;
						cv::split(next, BGR2);
						BGR2[0] = BGR2[0] + BGR2[1];
						BGR2[0] = BGR2[0] + BGR2[2];
						correlations_RGB[i] += abs(cv::mean(BGR2[0] - BGR1[0]).val[0]);
						
						count[i]++;
					}
					videoReader.set(CV_CAP_PROP_POS_FRAMES, j+1);
				}
			}
			ofstream ofstr_RGB("correlation_RGB.txt");
			ofstream ofstr("correlation.txt");
			for (int i = 1; i <= n; i++)
			{
				ofstr << 100*correlations[i] / (count[i]*255) << endl;
				ofstr_RGB << 100*correlations_RGB[i] / (count[i] * 255 * 3) << endl;
			}
			ofstr.close();
			ofstr_RGB.close();
		}
	}
	// compare two videos that should be identical
	static void compareVideos(string video1, string video2)
	{
		VideoCapture vid1(video1);
		VideoCapture vid2(video2);
		if (vid1.isOpened())
		{
			if (vid2.isOpened())
			{
				Mat frame1,frame2;
				while (vid1.read(frame1) && vid2.read(frame2))
				{
					imshow("diff", 255*(frame2 - frame1));
					cv::waitKey(0);
				}
			}
		}
	}

	// add dummy seconds to video
	static void addDummyFramesToVideo(VideoWriter &vidWriter, int n, Mat dummyFrame = Mat::zeros(getFrameSize(),CV_8UC3))
	{
		for (int i = 0; i < n; i++)
		{
			vidWriter << dummyFrame;
		}
	}

	// this method is combining the system parameters into a string to be used in the output video name
	static string createOuputVideoName(string inputMessage,string inputVideoFile,string outputVideoFile)
	{
		ostringstream outputVideoStream;
		outputVideoStream << inputMessage << "_" << Parameters::symbolsData.toString() << "_";
		outputVideoStream << Parameters::getSide() << "_";
		outputVideoStream << Parameters::getFull() << "_";
		outputVideoStream << Parameters::symbolTime << "ms_" << "levels_";
		outputVideoStream << Parameters::getCodec() << "_" << inputVideoFile << "_";
		string str = outputVideoStream.str();
		std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
		str.erase(end_pos, str.end());
		end_pos = std::remove(str.begin(), str.end(), '.');
		str.erase(end_pos, str.end());
		end_pos = std::remove(str.begin(), str.end(), '\\');
		str.erase(end_pos, str.end());
		return str + outputVideoFile;
	}

	static void extractOneFrameLuminance(Mat *add_mask, vector<cv::Rect> &ROIs, 
		vector<vector<float> > &frames,Mat &tmp_frame,Mat &tmp_prev,int i)
	{
		switch (Parameters::amplitudeExtraction)
		{
		case ALPHA_CHANNEL:
			Utilities::getIntensity(tmp_frame(ROIs[i]), frames[i], (*add_mask)(ROIs[i]));
			break;
		case V_CHANNEL:
			Utilities::getVchannel(tmp_frame(ROIs[i]), frames[i], (*add_mask)(ROIs[i]));
			break;
		case V_CHANNEL_DIFF:
			Utilities::getDiffInVchannelHSV(tmp_prev, tmp_frame, ROIs[i], frames[i], (*add_mask)(ROIs[i]));
			break;
		case BR_CHANNELS_DIFF:
			Utilities::getDiffBetweenFramesBR(tmp_prev, tmp_frame, ROIs[i], frames[i]);
			break;
		case BR_CHANNELS_SEPARATE:
			Utilities::getDiffBetweenFramesSeprateBR(tmp_prev, tmp_frame, ROIs[i], frames[i]);
			break;
		case BR_G_CHANNELS_DIFF:
			Utilities::getDiffBetweenFramesBR_G(tmp_prev, tmp_frame, ROIs[i], frames[i], (*add_mask)(ROIs[i]));
			break;
		}
	}

	/// get video frames luminance
	// VideoCapture as input
	// ROI as input
	// returns vector<float> with the luminances
	static vector<vector<float> > getVideoFrameLuminances(VideoCapture &cap, vector<cv::Rect> &ROIs, double fps, cv::Rect globalROI, bool oldMethod = false, bool useAlpha = true)
	{
		Parameters::endingIndex = Parameters::startingIndex + 1;
		vector<vector<float> > frames(ROIs.size());
		cout << "Processing Frames..." << endl;
		Mat frame, prev;
		cap.read(prev);
		double test_frame_rate = fps; // 30
		//cap.read(prev);
		//prev = prev(ROI);
		double nextIndex = 0; 
		int count = 1;
		//Mat mask, prev_mask; = getBinaryMask(prev);
		frame = prev.clone();
		int ROIsSize = ROIs.size();

		while (cap.read(frame))
		{
			//if (Parameters::synchMethod == SYNCH_CHESS)// && (count % 3) == 0)
			if (count & 3 && Parameters::synchMethod == SYNCH_CHESS)
			{
				//Mat temp;
				if (canDetectMyBoard(frame))
				{
					break;
				}
			}

			Mat add_mask = MaskFactory::getBackgroundMask(prev(globalROI), frame(globalROI));// prev_mask & mask;
			Mat tmp_frame_BGR;
			frame(globalROI).copyTo(tmp_frame_BGR, add_mask);
			Mat tmp_prev_BGR;
			prev(globalROI).copyTo(tmp_prev_BGR, add_mask);
			for (int i = 0; i < ROIsSize; i++)
			{
				extractOneFrameLuminance(0, ROIs, frames, tmp_frame_BGR, tmp_prev_BGR, i);
				
			}
			prev = frame.clone();
		}
		Parameters::endingIndex = cap.get(CV_CAP_PROP_POS_FRAMES);
		cout << "last index = " << Parameters::endingIndex << endl;
		return frames;
	}

	/// get video frames luminance with tracking color
	// VideoCapture as input
	// ROI as input
	// returns vector<float> with the luminances 
	static vector<vector<float> > getVideoFrameLuminancesWithColorTracking(
		VideoCapture cap, vector<cv::Rect> ROIs, double fps, bool useChessBoard, cv::Rect globalROI,cv::Scalar color)
	{
		vector<vector<float> > frames(ROIs.size());
		cout << "Processing Frames..." << endl;
		Mat frame, prev;
		cap.read(prev);
		//cap.read(prev);
		//prev = prev(ROI);
		double nextIndex = 0;
		int count = 1;
		//Mat mask, prev_mask; = getBinaryMask(prev);
		frame = prev.clone();
		while (true)
		{
			//imshow("prev", prev);
			//cv::waitKey(30);
			nextIndex += fps / 30;
			bool flag = true;

			while ((int)nextIndex > count + 1)
			{
				++count;
				flag = cap.read(frame);
			}
			if (!flag)
			{
				break;
			}
			//cout << count << endl;
			//mask = getBinaryMask(frame);
			if (useChessBoard && (count % 5) == 0)
			{
				Mat temp;
				cv::resize(frame, temp, cv::Size(640, 480));
				if (canDetectMyBoard(frame))
				{
					break;
				}
			}

			Mat add_mask = MaskFactory::getColorMask(frame(globalROI),color);// prev_mask & mask;
			Mat tmp_frame;
			frame(globalROI).copyTo(tmp_frame, add_mask);
			Mat tmp_prev;
			prev(globalROI).copyTo(tmp_prev, add_mask);
			for (int i = 0; i < ROIs.size(); i++)
			{
				Utilities::getDiffInVchannelHSV(tmp_prev, tmp_frame, ROIs[i], frames[i], add_mask(ROIs[i]));
			}
			prev = frame.clone();
		}
		return frames;
	}

	// divide a frame with certain number of divisions with removing percentage from the boundaries
	// divisions: number of divisions
	// frame_width: frame width
	// frame_height: frame Height
	// percent: percentage to crop from the image
	// cropInclusive: means crop this percentage from each section after dividing while false means crop this percentage from the whole frame then divide 
	static vector<cv::Rect> getDivisions(int sideWidth,int sideHeight,double percent,bool cropInclusive,cv::Rect globalROI,bool translateToOriginal,bool spatialRedundancy)
	{
		int frame_width, frame_height;
		frame_width = globalROI.width;
		frame_height = globalROI.height;
		vector<cv::Rect> ROIs;
		int sectionWidth = 0, sectionHeight = 0;
		if (cropInclusive)
		{
			sectionWidth = frame_width / sideWidth;
			sectionHeight = frame_height / sideHeight;
			
			for (int y = 0; y < sideHeight; y++)
			{
				for (int x = 0; x < sideWidth; x++)
				{
					// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
					ROIs.push_back(cv::Rect(
						x * sectionWidth + (1 - percent)*sectionWidth,
						y * sectionHeight + (1 - percent)*sectionHeight,
						percent * sectionWidth,
						percent * sectionHeight));
				}
			}

		}
		else
		{
			sectionWidth = (frame_width * percent) / sideWidth;
			sectionHeight = (frame_height * percent) / sideHeight;
			
			int widthStart = frame_width * (1 - percent);
			int heightStart = frame_height * (1 - percent);
			for (int y = 0; y < sideHeight; y++)
			{
				for (int x = 0; x < sideWidth; x++)
				{
					// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
					ROIs.push_back(cv::Rect(
						x * sectionWidth + widthStart,
						y * sectionHeight + heightStart,
						sectionWidth,
						sectionHeight));
				}
			}
		}
		if (translateToOriginal)
		{
			// shift to the global ROI
			for (int i = 0; i < ROIs.size(); i++)
			{
				ROIs[i].x += globalROI.x;
				ROIs[i].y += globalROI.y;
			}
		}
		if (spatialRedundancy)
		{
			vector<Rect> tempROIs;
			for (int i = 0; i < ROIs.size(); i++)
			{
				tempROIs.push_back(cv::Rect(ROIs[i].x, ROIs[i].y, ROIs[i].width / 2, ROIs[i].height));
				tempROIs.push_back(cv::Rect(ROIs[i].x + ROIs[i].width / 2, ROIs[i].y, ROIs[i].width / 2, ROIs[i].height));
			}
			return tempROIs;
		}
		return ROIs;
	}

	static float getMaxSum(float *arr, int startIncInput, int endExcInput, int &startOuput, int &endOutput)
	{
		float sum_here, sum_total;
		sum_here = sum_total = arr[startIncInput];
		int start = startOuput = startIncInput, end = endOutput = startIncInput + 1;
		for (int i = startIncInput + 1; i < endExcInput; i++)
		{
			if (arr[i] > sum_here + arr[i])
			{
				sum_here = arr[i];
				start = i;
				end = i + 1;
			}
			else if (sum_here + arr[i] > arr[i])
			{
				sum_here = sum_here + arr[i];
				end = i + 1;
			}
			
			if (sum_here > sum_total)
			{
				sum_total = sum_here;
				startOuput = start;
				endOutput = end;
			}
		}
		return sum_total;
	}

	// should be float 
	static cv::Rect getMaxSum(Mat &inp, float &maxSum)
	{
		int cols = inp.cols;
		int rows = inp.rows;
		int sz = rows * cols;
		float* data = (float*)inp.data;
		// Variables to store the final output
		maxSum = INT_MIN;
		int finalLeft, finalRight, finalTop, finalBottom;

		int top, bottom, i;
		float sum;
		int start, finish;
		float * temp = new float[cols];
		// Set the left column
		for (top = 0; top < rows; ++top)
		{
			// Initialize all elements of temp as 0
			memset(temp, 0, cols*sizeof(float));
			// Set the right column for the left column set by outer loop
			for (bottom = top; bottom < rows; ++bottom)
			{
				// Calculate sum between current left and right for every row 'i'
				for (i = 0; i < cols; ++i)
				{
					temp[i] += data[i + bottom * cols];
				}
				// Find the maximum sum subarray in temp[]. The kadane() function
				// also sets values of start and finish.  So 'sum' is sum of
				// rectangle between (start, left) and (finish, right) which is the
				//  maximum sum with boundary columns strictly as left and right.
				sum = getMaxSum(temp, 0, cols,  start, finish);

				// Compare sum with maximum sum so far. If sum is more, then update
				// maxSum and other output values
				if (sum > maxSum)
				{
					maxSum = sum;
					finalLeft = start;
					finalRight = finish;
					finalTop = top;
					finalBottom = bottom + 1;
				}
			}
		}
		delete[]temp;
		//cout << "maxSum = " << maxSum << endl;
		//printf("(%d\t%d)\t(%d\t%d)\n", finalLeft, finalTop, finalRight, finalBottom);
		return cv::Rect(finalLeft, finalTop, finalRight - finalLeft, finalBottom - finalTop);
	}

	// should be float 
	static cv::Rect getMaxSumDP(Mat &inp, float &maxSum)
	{
		int cols = inp.cols;
		int rows = inp.rows;
		int sz = rows * cols;
		float* data = (float*)inp.data;
		maxSum = FLT_MIN;
		// Variables to store the final output
		float*dp = new float[sz];
		memset(dp, 0, sz * sizeof(float));
		dp[0] = data[0];
		for (int c = 1; c < cols; c++)
		{
			dp[c] = data[c] + dp[c - 1];
		}
		for (int r = 1; r < rows; r++)
		{
			dp[r * cols] = data[r * cols] + dp[(r - 1) * cols];
			for (int c = 1; c < cols; c++)
			{
				dp[c + r * cols] = data[c + r * cols] + dp[c - 1 + r * cols] + dp[c + (r - 1) * cols] - dp[c - 1 + (r - 1) * cols];
			}
		}
		int finalLeft, finalRight, finalTop, finalBottom;
		for (int r1 = 0; r1 < rows; r1++)
		{
			for (int c1 = 0; c1 < cols; c1++)
			{
				for (int r2 = r1; r2 < rows; r2++)
				{
					for (int c2 = c1; c2 < cols; c2++)
					{
						float a = dp[r2 * cols + c2];
						float b = ((r1 > 0) ? dp[(r1 - 1) * cols + c2] : 0);
						float c = ((c1 > 0) ? dp[r2 * cols + c1 - 1] : 0);
						float d = (((c1 > 0) && (r1 > 0)) ? dp[(r1 - 1) * cols + c1 - 1] : 0);
						float sum = a - b - c + d;
						if (sum > maxSum)
						{
							maxSum = sum;
							finalLeft = c1;
							finalRight = c2 + 1;
							finalTop = r1;
							finalBottom = r2 + 1;
						}
					}
				}
			}
		}
		delete[]dp;
		//cout << "DP maxSum = " << maxSum << endl;
		//printf("(%d\t%d)\t(%d\t%d)\n", finalLeft, finalTop, finalRight, finalBottom);
		return cv::Rect(finalLeft, finalTop, finalRight - finalLeft, finalBottom - finalTop);
	}
	// this function is trying to find athe chess board and detect the last frame with the chess board and to detect the global ROI as well
	static cv::Rect getGlobalROI(VideoCapture &cap, int &starting_index)
	{
		cv::Rect globalROI(0, 0, 1, 1);
		if (!cap.isOpened())  // check if we succeeded
			return globalROI;
		double framerate = cap.get(CV_CAP_PROP_FPS);
		double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
		int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
		int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
		cap.set(CV_CAP_PROP_POS_FRAMES, framerate * Parameters::start_second); //Set index to last frame
		// try to detect the chess board
		Mat frame;
		globalROI.width = frame_width;
		globalROI.height = frame_height;
		cout << "we are here\n";
		if (Parameters::synchMethod == SYNCH_CHESS)
		{
			starting_index = 0;
			for (; cap.read(frame); starting_index++)
			{
				// this loop to detect the first chess board
				if (canDetectMyBoard(frame))
				{
					break;
				}
			}
			//cout << "first index = " << starting_index << endl;
			int countChess = 0;
			int countErrors = 0;
			for (; cap.read(frame) && countErrors < framerate; starting_index++, countChess++)
			{
				//cout << "found chess at index = " << starting_index << endl;
				// this loop to detect the last chess board
				if (canDetectMyBoard(frame))
				{
					globalROI = detectMyBoard(frame);
					countErrors = 0;
				}
				else
				{
					countErrors++;
				}
			}
			starting_index = starting_index - countErrors + 1;
		}
		else if (Parameters::synchMethod == SYNCH_GREEN_CHANNEL)
		{
			
			//int sz = frame_height * frame_width;
			//int* accData = (int*)accumelation.data;
			//Mat temp = Mat::zeros(frame_height, frame_width, CV_32FC1);
			cv::Size size = getFrameSize();
			int width = frame_width;// getFrameSize().width;
			int height = frame_height;// getFrameSize().height;
			cv::Rect tempROI = cv::Rect(0, 0, frame_width, frame_height);
			vector<double> diffMaxSumBox;
			vector<double> accMaxSumBox;
			vector<cv::Rect> maxSumBox;
			vector<int> candidateIndex;
			cout << "GREEN\n";
			Mat prev, frame, accumelation = Mat::zeros(frame_height, frame_width, CV_32FC1);
			if (cap.read(prev))
			{
				//cv::resize(prev,prev,size);
				Mat tmp_mask = Mat::zeros(prev.size(), CV_8UC1) + 255;
				int index = 1;
				while (cap.read(frame))
				{
					index++;
					//cv::resize(frame, frame, size);
					//cout << "Frame Index = " << << endl;
					//frame.convertTo(frame, CV_32FC1);
					vector<float> val;
					
					Mat temp = getDiffBetweenFramesBR_G(prev, frame, tempROI,val,tmp_mask);
					if (val[0] < 0)
					{
						temp = -temp;
					}
					//cv::erode(temp, temp, Mat());
					//cv::dilate(temp, temp, Mat());
					//cv::erode(temp, temp, Mat());
					//cv::dilate(temp, temp, Mat());
					
					accumelation = accumelation + temp - 0.5;
					// for testing only
					{
						Mat bkg = Mat::zeros(accumelation.size(), CV_8UC1);
						cv::threshold(accumelation, bkg, 0, 1, THRESH_BINARY);
						imshow("bkg", bkg);
						cv::waitKey(10);
					}
					//Mat temp1;
					//cv::threshold(accumelation, temp1, 0, 1, THRESH_BINARY);
					//Mat ret = temp1 * 255;
					//double minn;
					//cv::minMaxIdx(accumelation, &minn, 0);
					//accumelation = accumelation - minn - 1;
					float maxSumVal;
					maxSumBox.push_back(getMaxSum(accumelation, maxSumVal));
					cout << "Index = " << index << endl;
					cout << maxSumVal << endl;
					globalROI = *maxSumBox.rbegin();
					printf("(%d\t%d)\t(%d\t%d)\n", globalROI.x, globalROI.y, globalROI.width, globalROI.height);
					accMaxSumBox.push_back(maxSumVal);
					if (index > framerate / 2)
					{
						int compare = std::min((int)framerate, index);
						int errors = 0;
						for (int i = index - compare + 1; i < accMaxSumBox.size(); i++)
						{
							if (accMaxSumBox[i] < accMaxSumBox[i - 1] && accMaxSumBox[i - 1] < accMaxSumBox[i - 2])
							{
								errors++;
							}
						}
						cout << "Errors = " << errors << endl;
						if (errors < 2 && accMaxSumBox[index - 2] > accMaxSumBox[index - 3])
						{
							candidateIndex.push_back(index);
						}
						else if (candidateIndex.size() > 0)
						{
							starting_index = *candidateIndex.rbegin() - 3;
							globalROI = maxSumBox[starting_index - 2]; // because it is zeros based index and index is one more than size
							if (globalROI.width >= width / 2 && globalROI.height >= height / 2)
							{
								// and move it back to its original location
								float colScale = ((float)frame_width) / width;
								float rowScale = ((float)frame_height) / height;

								globalROI.x = globalROI.x * colScale;
								globalROI.y = globalROI.y * rowScale;
								globalROI.width = globalROI.width * colScale;
								globalROI.height = globalROI.height * rowScale;
								// and break
								break;
							}
						}
					}
					//diffMaxSumBox.push_back(cv::sum(temp(*maxSumBox.rbegin())).val[0]);
					//int count = 0;
					//double avg = 0;
					//for (int i = max(0, ((int)diffMaxSumBox.size()) - 6); i < (int)diffMaxSumBox.size(); i++)
					//{
					//	avg += diffMaxSumBox[i];
					//	count++;
					//}
					//cout << (int)(avg / count) << endl;
					//getMaxSumDP(temp1, maxSumVal);
					//ret.convertTo(test, CV_8UC1);
					//Mat test = frame.clone();
					
					cv::rectangle(prev, *maxSumBox.rbegin(), cv::Scalar(255, 0, 0), 4);
					//cv::resize(prev, prev, Parameters::DefaultFrameSize);
					imshow("frame", prev);
					//cv::resize(ret, ret, Parameters::DefaultFrameSize);
					//imshow("acc", ret);
					cv::waitKey(10);
					//
					prev = frame.clone();
				}
			}
		}
		printf("(%d\t%d)\t(%d\t%d)\n", globalROI.x, globalROI.y, globalROI.width, globalROI.height);
		return globalROI;
	}
	/// get video frames luminance (this is the split version which splits the image into two)
	// video name as input
	// percentage of the frame as input (used to get this percentage from the center of the image) and takes value from (0,1]
	// int &framerate: is output parameter
	// divisions: supports 2 and 4 only for now
	// ROI: is the area of interest only for the whole image
	static vector<vector<float> > getVideoFrameLuminancesSplitted(string videoName, double percent, int &framerate, int sideA,int sideB,bool useGlobalROI,
		bool spatialRedundancy,cv::Scalar color = cv::Scalar(-1,-1,-1))
	{
		vector<vector<float> > frames;
		VideoCapture cap(videoName); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
			return frames;
		framerate = cap.get(CV_CAP_PROP_FPS); //get the frame rate
		// try to detect the chess board
		Parameters::globalROI = cv::Rect(0, 0, cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));
		if (useGlobalROI)
		{
			Parameters::globalROI = getGlobalROI(cap, Parameters::startingIndex);
		}
		Parameters::startingIndex += Parameters::start_second * framerate;
		cap.set(CV_CAP_PROP_POS_FRAMES, Parameters::startingIndex);
		cout << "Index = " << Parameters::startingIndex << endl;
		// the ROI		
		vector<cv::Rect> ROIs = getDivisions(sideA,sideB, percent, false, Parameters::globalROI, false, spatialRedundancy);
		if (color == cv::Scalar(-1, -1, -1))
		{
			frames = getVideoFrameLuminances(cap, ROIs, framerate, Parameters::globalROI);
		}
		else
		{
			frames = getVideoFrameLuminancesWithColorTracking(cap, ROIs, framerate, true, Parameters::globalROI, color);
		}
		if (Parameters::endSecondFile.length() > 0)
		{
			ofstream endSecond(Parameters::endSecondFile);
			endSecond << (Parameters::endingIndex * 1.0) / framerate;
			endSecond.close();
		}
		//framerate = 30; // because we sample at 30 fps
		if (((Parameters::endingIndex - Parameters::startingIndex) / framerate) < 3)
		{
			Parameters::start_second = cap.get(CV_CAP_PROP_POS_MSEC) * 1.0 / 1000.0;
			return getVideoFrameLuminancesSplitted(videoName, percent, framerate, sideA,sideB, useGlobalROI, spatialRedundancy, color);
		}
		return frames;
	}

	static vector<float> getVideoFrameLuminancesOLd(string videoName, int &framerate, bool useGlobalROI,bool useAlpha)
	{
		cout << "Old method" << endl;
		VideoCapture cap(videoName); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
			return vector<float>();
		framerate = cap.get(CV_CAP_PROP_FPS); //get the frame rate
		// try to detect the chess board
		int index = 0;
		cv::Rect globalROI(0, 0, cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));
		vector<cv::Rect> ROIs;
		
		if (useGlobalROI)
		{
			globalROI = getGlobalROI(cap, index);
			cap.set(CV_CAP_PROP_POS_FRAMES, index);
		}
		ROIs.push_back(globalROI);
		ROIs[0].x = ROIs[0].y = 0;
		cout << "Index = " << index << endl;
		if (useAlpha)
		{
			cout << "Alpha" << endl;
		}
		else
		{
			cout << "V-channel" << endl;
		}
		return getVideoFrameLuminances(cap, ROIs, framerate, globalROI, true, useAlpha)[0];
	}

	float getLuminanceWithMaskFromGray(Mat& frame, cv::Rect& ROI)
	{
		// get the ROI
		Mat tmp = frame(ROI);
		// convert the color for the ROI only
		cv::cvtColor(tmp, tmp, CV_BGR2GRAY);
		// get pixels with values above 127
		cv::Mat mask = (tmp > 10) / 255;
		//imshow("test", tmp.mul(mask));
		//cv::waitKey(0);
		return cv::sum(tmp.mul(mask)).val[0] / cv::sum(mask).val[0];
	}

	float getLuminanceWithMaskFromHSV(Mat& frame, cv::Rect& ROI, double percent)
	{
		// get the ROI
		Mat tmp = frame(ROI);
		// convert the color for the ROI only
		// convert the color for the ROI only
		cv::cvtColor(tmp, tmp, CV_BGR2HSV);
		vector<Mat> HSV(3);
		split(tmp, HSV);
		// get pixels with values above 127
		Mat tmp1 = HSV[2].clone();
		cv::Mat mask = (tmp1 > (255 - 255 * percent)) / 255;
		//imshow("test", tmp.mul(mask));
		//cv::waitKey(0);
		return cv::sum(tmp1.mul(mask)).val[0] / cv::sum(mask).val[0];
	}

	float getLuminanceFromGray(Mat& frame, cv::Rect& ROI)
	{
		// get the ROI
		Mat tmp = frame(ROI);
		// convert the color for the ROI only
		cv::cvtColor(tmp, tmp, CV_BGR2GRAY);
		cv::Scalar tempVal = mean(tmp);
		float myMAtMean = tempVal.val[0];

		return myMAtMean;
	}

	float getLuminanceFromHSV(Mat& frame, cv::Rect& ROI)
	{
		// get the ROI
		Mat tmp = frame(ROI);
		// convert the color for the ROI only
		cv::cvtColor(tmp, tmp, CV_BGR2HSV);
		vector<Mat> HSV(3);
		split(tmp, HSV);
		cv::Scalar tempVal = mean(HSV[2]);
		float myMAtMean = tempVal.val[0];

		return myMAtMean;
	}

	float getLuminance(Mat& frame, cv::Rect& ROI)
	{
		return getLuminanceWithMaskFromHSV(frame, ROI, 1);
	}

	/// get video frames luminance
	// video name as input
	// percentage of the frame as input (used to get this percentage from the center of the image) and takes value from (0,1]
	// int &framerate: is out parameter
	static vector<float> getVideoFrameLuminances(string videoName, int &framerate)
	{
		vector<float> frames;
		VideoCapture cap(videoName); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
			return frames;
		double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
		framerate = cap.get(CV_CAP_PROP_FPS); //get the frame rate
		int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
		int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
		cout << count << endl;
		//Mat edges;
		//namedWindow("edges", 1);
		cout << "Processing Frames..." << endl;
		cap.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
		int ind = 0;
		Mat frame;
		bool success = cap.read(frame);
		vector<Point> points = SelectByMouse::getROI(frame);
		int lx = 10000000, ly = 10000000, hx = 0, hy = 0;
		for (int i = 0; i < points.size(); i++)
		{
			lx = min(lx, points[i].x);
			ly = min(ly, points[i].y);
			hx = max(hx, points[i].x);
			hy = max(hy, points[i].y);
		}
		cv::Rect ROI(lx, ly, hx - lx + 1, hy - ly + 1);
		vector<cv::Rect> ROIs;
		ROIs.push_back(ROI);
		return Utilities::getVideoFrameLuminances(cap, ROIs, framerate, cv::Rect(0, 0, frame_width, frame_height))[0];
	}

	/// get video frames luminance
	// video name as input
	// percentage of the frame as input (used to get this percentage from the center of the image) and takes value from (0,1]
	// int &framerate: is output parameter
	static vector<float> getVideoFrameLuminances(string videoName, double percent, int &framerate)
	{
		vector<float> frames;
		VideoCapture cap(videoName); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
			return frames;
		double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
		framerate = cap.get(CV_CAP_PROP_FPS); //get the frame rate
		int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
		int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
		// the ROI
		int width = frame_width * percent;
		int height = frame_height * percent;
		int lowerX = (frame_width - width) / 2;
		int lowerY = (frame_height - height) / 2;
		VideoWriter vidWriter;
		vidWriter.open(std::to_string(percent) + ".avi", CV_FOURCC('D', 'I', 'V', 'X'), framerate, cv::Size(width, height));
		cap.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
		vector<cv::Rect> ROI;
		ROI.push_back(cv::Rect(lowerX, lowerY, width, height));
		return Utilities::getVideoFrameLuminances(cap, ROI, framerate, cv::Rect(0, 0, frame_width, frame_height))[0];
	}

	// open a video writer to use the same codec with every one
	static VideoWriter getVideoWriter(string vidName,cv::Size frameSize)
	{
		// 0 -> no compression, DIVX, H264
		unsigned long milliseconds_since_epoch =
			std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::system_clock::now().time_since_epoch()).count();
		ostringstream name;
		name << milliseconds_since_epoch << "_" << vidName;
		string codec = Parameters::getCodec();
		VideoWriter vidWriter(name.str(),
			CV_FOURCC(codec[0], codec[1], codec[2], codec[3])
			, Parameters::fps, frameSize);
		return vidWriter;
	}
	static cv::Size getFrameSize()
	{
		return Parameters::DefaultFrameSize;
	}

	// calculate longest common subsequence between two strings
	static void LCS(vector<short> orig_msg, vector<short> test_msg)
	{
		int **l = new int*[orig_msg.size() + 1];
		//int lcs = -1;

		for (int i = 0; i <= orig_msg.size(); i++)
		{
			l[i] = new int[test_msg.size() + 1];
			for (int j = 0; j <= test_msg.size(); j++)
			{
				l[i][j] = 0;
			}
		}
		
		printf("%d and %d\r\n", orig_msg.size(), test_msg.size());
		for (int i = 1; i <= orig_msg.size(); i++)
		{
			for (int j = 1; j <= test_msg.size(); j++)
			{
				l[i][j] = std::max(l[i - 1][j - 1], std::max(l[i - 1][j], l[i][j - 1]));
				if (orig_msg[i - 1] == test_msg[j - 1])
				{
					l[i][j] = max(l[i - 1][j - 1] + 1, max(l[i - 1][j], l[i][j - 1]));
				}
			}
		}
		int lcs = l[orig_msg.size()][test_msg.size()];
		double percent = lcs;
		percent /= orig_msg.size();
		printf("Longest Common SubString Length = %d = %0.2llf%%\r\n", lcs, 100*percent);
		// deallocate
		for (int i = 0; i <= orig_msg.size(); i++)
		{
			delete []l[i];
		}
		delete []l;
	}

	// calculate best greedy match
	static void LCS_greedy(vector<short> orig_msg, vector<short> test_msg,string videoName = "")
	{
		printf("%d and %d\r\n", orig_msg.size(), test_msg.size());
		int lcs = 0;
		int t_sz = test_msg.size();
		int o_sz = orig_msg.size();
		int best_i;
		vector<int> errors;
		//for (int i = 1 - t_sz; i < o_sz; i++)
		for (int i = 0; i < 1; i++)
		{
			int sum = 0;
			vector<int> tempErrors;
			for (int j = std::max(0, i); j < std::min(o_sz, t_sz + i); j++)
			{
				if (orig_msg[j] ^ test_msg[j - i])
				{
					tempErrors.push_back(j - i);
				}
				else
				{
					++sum;
				}
			}
			//cout << i << "\t" << sum << endl;
			if (sum > lcs)
			{
				lcs = sum;
				best_i = i;
				errors = tempErrors;
			}
		}
		double percent = lcs;
		percent /= orig_msg.size();
		printf("Longest Common SubString at index = %d Length = %d = %0.2llf%%\r\n", best_i, lcs, 100 * percent);
		if (videoName.size() > 0)
		{
			VideoCapture videoReader;
			videoReader.open(videoName);
			if (videoReader.isOpened())
			{
				Mat frame;
				int numberOfFrames = videoReader.get(CV_CAP_PROP_FRAME_COUNT); // get frame count
				int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
				int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
				int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);

				puts("Errors:");
				
				// for testing the correctness of this only
				errors.clear();
				for (int i = 0; i < errors.size(); i++)
				{
					ostringstream outputName;
					outputName << std::setfill('0') << std::setw(10);
					int start_data = 30 * (errors[i] + 1);
					int end_data = start_data + 30;
					int start_second = framerate * (errors[i] + 1);
					int end_second = start_second + framerate;
					outputName << start_second / framerate << "_" << end_second / framerate << "_" << orig_msg[errors[i]] << "_" << test_msg[errors[i]];
					VideoWriter vidWriter;
					string outVideo = outputName.str() + ".avi";
					string outFrameValues = outputName.str() + ".frames";
					//cout << "ROI: (" << Parameters::globalROI.x << "," << Parameters::globalROI.y << ")\t(";
					//cout << Parameters::globalROI.width << "," << Parameters::globalROI.height << ")\n";
					videoReader.set(CV_CAP_PROP_POS_FRAMES, start_second + Parameters::startingIndex);
					vidWriter.open(outVideo, CV_FOURCC('D', 'I', 'V', 'X'), framerate, cv::Size(Parameters::globalROI.width, Parameters::globalROI.height));
					for (int j = 0; j <= framerate; j++)
					{
						videoReader >> frame;
						vidWriter << frame(Parameters::globalROI);
					}
					ofstream outFrames(outFrameValues);
					for (int j = start_data; j <= end_data; j++)
					{
						outFrames << Parameters::amplitudes[j] << endl;
					}
					outFrames.close();
					printf("%d\n", errors[i]);
				}
			}
		}
	}

	// create binary message from string message
	static vector<short> getBinaryMessage(string msg)
	{
		vector<short> result;
		bool convert = false;
		for (int i = 0; i < msg.length(); i++)
		{
			if (msg[i] != '0' && msg[i] != '1')
			{
				convert = true;
				break;
			}
		}
		if (convert)
		{
			for (int i = 0; i < msg.length(); i++)
			{
				for (int j = 7; j >= 0; j--)
				{
					result.push_back((msg[i] >> (7 - j)) & 1);
				}
			}
		}
		else
		{
			for (int i = 0; i < msg.length(); i++)
			{
				result.push_back(msg[i] - '0');
			}
		}
		return result;
	}
	// Get teh size from utilities
	// boarderPercentage that will be white
	// patternsize is the number of interior points
	// return BGR image
	static Mat createChessBoard()
	{
		double boarderPercentage = 0.95;
		Mat board = 255 * Mat::ones(Utilities::getFrameSize(), CV_8UC1);
		cv::cvtColor(board, board, CV_GRAY2BGR);
		int xStep = (board.cols * boarderPercentage) / (Parameters::patternsize.width + 1);
		int yStep = (board.rows * boarderPercentage) / (Parameters::patternsize.height + 1);
		int xStart = (board.cols * (1 - boarderPercentage)) / 2;
		int yStart = (board.rows * (1 - boarderPercentage)) / 2;
		for (int x = 0; x <= Parameters::patternsize.width; x += 2)
		{
			for (int y = 0; y <= Parameters::patternsize.height; y += 2)
			{
				cv::rectangle(board,
					cv::Point(xStart + x*xStep, yStart + y*yStep),
					cv::Point(xStart + (x + 1)*xStep, yStart + (y + 1)*yStep),
					cv::Scalar(0, 0, 0),
					CV_FILLED);

			}
		}
		for (int x = 1; x <= Parameters::patternsize.width; x += 2)
		{
			for (int y = 1; y <= Parameters::patternsize.height; y += 2)
			{
				cv::rectangle(board,
					cv::Point(xStart + x*xStep, yStart + y*yStep),
					cv::Point(xStart + (x + 1)*xStep, yStart + (y + 1)*yStep),
					cv::Scalar(0, 0, 0),
					CV_FILLED);

			}
		}

		return board;
	}

	static void convertImgtoGray(Mat &img, Mat &gray)
	{
		if (img.cols > 640 && img.rows > 480)
		{
			gray = cv::Mat::zeros(cv::Size(640, 480), CV_8UC1); //source image
		}
		else
		{
			gray = cv::Mat::zeros(img.size(), CV_8UC1); //source image	
		}

		int channels = img.channels();
		int cols = gray.cols;
		int rows = gray.rows;
		int imgCols = img.cols;
		int imgRows = img.rows;
		int index = 0;
		unsigned char* imgData = (unsigned char*)img.data;
		unsigned char* grayData = (unsigned char*)gray.data;
		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < cols; c++)
			{
				grayData[index++] = imgData[(imgCols * ((r * imgRows) / rows) + ((c * imgCols) / cols)) * channels];
			}
		}
	}

	// img: input image in BGR
	// patternSize: interior number of corners
	// return rectangle around the calibration points
	static cv::Rect detectMyBoard(Mat &img)
	{
		// create the gray image
		Mat gray; //source image
		//convertImgtoGray(img, gray);
		if (img.channels() == 3)
		{
			cv::cvtColor(img, gray, CV_BGR2GRAY);
		}
		else
		{
			gray = img.clone();
		}

		cv::resize(gray, gray, cv::Size(640, 480));
		
		vector<Point2f> corners; //this will be filled by the detected corners

		//CALIB_CB_FAST_CHECK saves a lot of time on images
		//that do not contain any chessboard corners
		bool patternfound = findChessboardCorners(gray, Parameters::patternsize, corners,
			CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE
			+ CALIB_CB_FAST_CHECK);

		cv::Rect result;
		if (patternfound)
		{
			cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
				TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
		}
		else
		{
			return result;
		}
		/*
		Mat temp = img.clone();
		drawChessboardCorners(temp, patternsize, Mat(corners), patternfound);
		imshow("img", temp);
		cv::waitKey(0);
		*/
		
		float xl = 100000, yl = 1000000, xh = 0, yh = 0;
		for (int i = 0; i < corners.size(); i++)
		{
			xl = std::min(xl, corners[i].x);
			yl = std::min(yl, corners[i].y);
			xh = std::max(xh, corners[i].x);
			yh = std::max(yh, corners[i].y);
		}
		float colScale = ((float)img.cols) / gray.cols;
		float rowScale = ((float)img.rows) / gray.rows;
		
		result.x = xl * colScale;
		result.y = yl * rowScale;
		result.width = (xh - xl + 1) * colScale;
		result.height = (yh - yl + 1) * rowScale;
		// then scale to full screen if required to do so
		if (Parameters::fullScreen)
		{
			double cellWidth = (result.width * 1.0) /(Parameters::patternsize.width - 1);
			double cellHeight = (result.height * 1.0) / (Parameters::patternsize.height - 1);
			result.x -= cellWidth;
			result.y -= cellHeight;
			result.width += cellWidth * 2;
			result.height += cellHeight * 2;
		}
		/*cv::rectangle(img, result, cv::Scalar(255, 0, 0), 2);
		imshow("img", img);
		cv::waitKey(0);*/
		return result;
	}


	// img: input image in BGR
	// return true if board detected
	static bool canDetectMyBoard(Mat &img)
	{
		// create the gray image
		Mat gray;//source image
		//convertImgtoGray(img, gray);
		//imshow("chess", gray);
		//cv::waitKey(0);
		
		if (img.channels() == 3)
		{
			cv::cvtColor(img, gray, CV_BGR2GRAY);
		}
		else
		{
			gray = img.clone();
		}
		cv::resize(gray, gray, cv::Size(640, 480));
		
		vector<Point2f> corners; //this will be filled by the detected corners

		//CALIB_CB_FAST_CHECK saves a lot of time on images
		//that do not contain any chessboard corners
		return findChessboardCorners(gray, Parameters::patternsize, corners,
			CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE
			+ CALIB_CB_FAST_CHECK);
	}

	/*
	* I am using this function for testing
	* trying to explore the video frames and see what are the parts with the major source of error
	*/
	static void exploreVideo(string videoName)
	{
		VideoCapture cap(videoName);
		
		if (cap.isOpened())
		{
			Mat frame;
			int index = 0;
			bool flag = true;
			while (true)
			{
				cap.set(CV_CAP_PROP_POS_FRAMES, index);
				cap >> frame;
				cv::resize(frame, frame, getFrameSize());
				imshow("frame", frame);
				// check the color mask as well
				MaskFactory::getColorMask(frame, cv::Scalar(0, 0, 230));
				cout << index << endl;
				int key = cv::waitKey(0);
				int new_index = 0;
				switch (key)
				{
				case 27:
					flag = false;
					break;
				case 2555904:
					index++;
					break;
				case 2424832:
					index--;
					break;
				default:
					while (key >= '0' && key <= '9')
					{
						new_index = (new_index * 10) + (key - '0');
						key = cv::waitKey(0);
					}
					index = new_index;
					break;
				}
			}
		}
		else
		{
			puts("Can not open the video file!");
		}
	}
	// this method is used to get the video frames as if it was at some required frame rate like 30fps
	static vector<int> getFrameIndeces(double original_fps, double fps, int number_frames)
	{
		vector<int> result;
		double diff = original_fps / fps;
		double index = 0;
		for (int i = 0; i < number_frames; i++, index += diff)
		{
			result.push_back(index);
		}
		return result;
	}
	static Mat calcHueHistogram(Mat &src, Mat &hueVal,int hbins = 30)
	{
		Mat hsv;

		cvtColor(src, hsv, CV_BGR2HSV);

		// Quantize the hue to 30 levels
		// and the saturation to 32 levels
		
		int histSize[] = { hbins };
		// hue varies from 0 to 179, see cvtColor
		float hranges[] = { 0, 180 };

		const float* ranges[] = { hranges };
		MatND hist;
		// we compute the histogram from the 0-th and 1-st channels
		int channels[] = { 0 };
		Mat HSV[3];
		cv::split(hsv, HSV);
		hueVal = HSV[0].clone();
		calcHist(&hsv, 1, channels, Mat(), // do not use mask
			hist, 1, histSize, ranges,
			true, // the histogram is uniform
			false);
		return hist;
	}
};
