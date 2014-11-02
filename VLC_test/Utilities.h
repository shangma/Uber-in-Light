#pragma once

#include "Header.h"
#include "SelectByMouse.h"

// 0 means 20 hz and 1 is 30 hz
const double FREQ[] = { 15, 10 };
const double LUMINANCE[] = { 0.005, -0.005};
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
		//cout << (int)(HSV[2].at<unsigned char>(0, 0)) << "\t";
		//HSV[2] = 0 * HSV[2] + (alpha / 100.0) * 255;
		// convert to float and add the difference as percentage
		//HSV[2].convertTo(HSV[2], CV_32F);
		// normalize
		//HSV[2] /= 255.0;
		Mat aux = HSV[2](ROI);
		// add the alpha value (which should be percentage)
		aux = (aux + alpha*255);
		// convert back to unsigned char
		//HSV[2] *= 255;
		//HSV[2].convertTo(HSV[2], CV_8U);
		//cout << (int)(HSV[2].at<unsigned char>(0, 0)) << endl;
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

	// the input frames here are the original frames
	static Mat getDiffInVchannelHSV(Mat &prev,Mat &frame,int radius)
	{
		// save the ROI
		Mat tmp, hsv1, hsv2;
		vector<Mat> HSV1, HSV2;
		Mat tmp1, tmp2;

		cv::cvtColor(prev, hsv1, CV_BGR2HSV);
		cv::split(hsv1, HSV1);
		HSV1[2].convertTo(tmp1, CV_32F);
		
		cv::cvtColor(frame, hsv2, CV_BGR2HSV);

		cv::split(hsv2, HSV2);

		HSV2[2].convertTo(tmp2, CV_32F);
		cv::subtract(tmp2, tmp1, tmp);
		
		//double min, max;
		//cv::minMaxLoc(tmp, &min, &max);
		//cout << min << "\t" << max << endl;
		/*
		tmp = tmp1.clone();
		int ind = 0;
		for (int y = 0; y < tmp.rows; y++)
		{
			for (int x = 0; x < tmp.cols; x++,ind++)
			{
				float val = getSmallestDifference(tmp1, tmp2, radius, x, y);
				if (abs(val > 3))
				{
					((float*)tmp.data)[ind] = 0;
				}
				else
				{
					((float*)tmp.data)[ind] = val;
				}
			}
		}
		*/
		return tmp;
	}

	// convert video fps to certain given fps
	static void convertVideo(string inputVideo, string outputVideo, double newFPS)
	{
		VideoCapture videoReader(inputVideo);
		if (videoReader.isOpened())
		{
			videoReader.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
			int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
			int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
			while (frame_height > 1000)
			{
				frame_width /= 2;
				frame_height /= 2;
			}
			VideoWriter vidWriter;
			vidWriter.open(outputVideo, CV_FOURCC('D', 'I', 'V', 'X'), newFPS, cv::Size(frame_width, frame_height));
			Mat frame;
			while (videoReader.read(frame))
			{
				Mat tmp;
				cv::resize(frame, tmp, cv::Size(frame_width, frame_height));
				vidWriter << tmp;
			}
		}
	}

	// this method is combining the system parameters into a string to be used in the output video name
	static string createOuputVideoName(int symbol_time,string inputVideoFile,string outputVideoFile)
	{
		ostringstream outputVideoStream;
		outputVideoStream << "_" << FREQ[ZERO] << "Hz_" << FREQ[ONE] << "Hz_";
		outputVideoStream << symbol_time << "ms_" << (LUMINANCE[0]-LUMINANCE[1]) << "levels_" << inputVideoFile << "_" << outputVideoFile;

		return outputVideoStream.str();
	}

	/// get video frames luminance
	// VideoCapture as input
	// ROI as input
	// returns vector<float> with the luminances
	static vector<float> getVideoFrameLuminances(VideoCapture cap, cv::Rect ROI)
	{
		vector<float> frames;
		cout << "Processing Frames..." << endl;
		Mat frame, prev;
		cap.read(prev);
		prev = prev(ROI);
		while (cap.read(frame))
		{
			frame = frame(ROI);
			// save the ROI
			Mat tmp = Utilities::getDiffInVchannelHSV(prev, frame, 0);
			//imshow("test", tmp);
			//cv::waitKey(0);
			float luminance = cv::mean(tmp).val[0];
			prev = frame.clone();
			//float luminance = getLuminance(tmp, ROI);
			frames.push_back(luminance);
		}
		// the camera will be deinitialized automatically in VideoCapture destructor
		return frames;
	}

	/// get video frames luminance (this is the split version which splits the image into two)
	// video name as input
	// percentage of the frame as input (used to get this percentage from the center of the image) and takes value from (0,1]
	// int &framerate: is output parameter
	// divisions: supports 2 and 4 only for now
	static vector<vector<float> > getVideoFrameLuminancesSplitted(string videoName, double percent, int &framerate, int divisions)
	{
		vector<vector<float> > frames;
		VideoCapture cap(videoName); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
			return frames;
		double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
		framerate = cap.get(CV_CAP_PROP_FPS); //get the frame rate
		int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
		int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
		// the ROI		
		if (divisions == 2)
		{
			int width = frame_width * percent;
			int height = frame_height * percent;
			int lowerX = (frame_width - width) / 2;
			int lowerY = (frame_height - height) / 2;

			cv::Rect ROI1 = cv::Rect(lowerX, lowerY, width / 2 - lowerX, height);
			cv::Rect ROI2 = cv::Rect(lowerX + frame_width / 2, lowerY, width / 2 - lowerX, height);

			cap.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			frames.push_back(getVideoFrameLuminances(cap, ROI1));
			cap.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			frames.push_back(getVideoFrameLuminances(cap, ROI2));
		}
		else if (divisions == 4)
		{
			int width = frame_width / 2;
			int height = frame_height / 2;
			int xStart = width * (1.0 - percent);
			int yStart = height * (1.0 - percent);

			cv::Rect ROI11 = cv::Rect(xStart, yStart, width * percent, height * percent);
			cap.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			frames.push_back(getVideoFrameLuminances(cap, ROI11));

			cv::Rect ROI12 = cv::Rect(xStart + width, yStart, width * percent, height * percent);
			cap.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			frames.push_back(getVideoFrameLuminances(cap, ROI12));

			cv::Rect ROI21 = cv::Rect(xStart, yStart + height, width * percent, height * percent);
			cap.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			frames.push_back(getVideoFrameLuminances(cap, ROI21));

			cv::Rect ROI22 = cv::Rect(xStart + width, yStart + height, width * percent, height * percent);
			cap.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			frames.push_back(getVideoFrameLuminances(cap, ROI22));
		}
		return frames;
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
		return Utilities::getVideoFrameLuminances(cap, ROI);
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
		cv::Rect ROI = cv::Rect(lowerX, lowerY, width, height);
		return Utilities::getVideoFrameLuminances(cap, ROI);
	}

	// open a video writer to use the same codec with every one
	static VideoWriter getVideoWriter(string vidName,double framerate, cv::Size frameSize)
	{
		VideoWriter vidWriter(vidName, 0/*CV_FOURCC('D', 'I', 'V', 'X')*/, framerate, frameSize);
		return vidWriter;
	}
};