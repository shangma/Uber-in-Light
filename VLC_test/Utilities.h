#pragma once

#include "Header.h"
#include "SelectByMouse.h"
#include "background_subtractor.h"
#include "ReceiveParameters.h"

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
	static void updateFrameLuminance(Mat &frame, Rect ROI, double percentage, bool useAlpha = false)
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

	static void updateFrameWithVchannel(Mat &frame, Rect ROI, double percentage)
	{
		Mat aux = frame(ROI);
		aux = (aux + percentage * 255);
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
		return lcm(inputFps, Utilities::lcm(2 * FREQ[ZERO], 2 * FREQ[ONE]));
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

	static Mat getVchannel(Mat frame)
	{
		vector<Mat> BGR1;
		cv::split(frame, BGR1);
		cv::max(BGR1[0], BGR1[1], BGR1[1]);
		cv::max(BGR1[1], BGR1[2], BGR1[2]);

		return BGR1[2];
	}
	static Mat getIntensity(Mat frame)
	{
		// save the ROI
		Mat tmp;
		/*frame.convertTo(tmp, CV_32F);
		vector<Mat> BGR;
		cv::split(frame, BGR);
		BGR[0] = BGR[0] + BGR[1];
		BGR[0] = BGR[0] + BGR[2];
		return BGR[0];*/
		
		cv::cvtColor(frame, tmp, CV_BGR2GRAY);
		return tmp;
	}
	// the input frames here are the original frames
	static Mat getDiffInVchannelHSV(Mat prev,Mat frame,int radius)
	{
		// save the ROI
		//Mat tmp, hsv1, hsv2;
		//vector<Mat> HSV1, HSV2;
		Mat tmp1, tmp2, tmp;

		//cv::cvtColor(prev, hsv1, CV_BGR2HSV);
		//cv::split(hsv1, HSV1);
		//HSV1[2].convertTo(tmp1, CV_32F);
		//cv::cvtColor(frame, hsv2, CV_BGR2HSV);
		//cv::split(hsv2, HSV2);
		//HSV2[2].convertTo(tmp2, CV_32F);
		//cv::subtract(tmp2, tmp1, tmp);
		vector<Mat> BGR1, BGR2;
		cv::split(prev, BGR1);
		cv::split(frame, BGR2);
		cv::max(BGR1[0], BGR1[1], BGR1[1]);
		cv::max(BGR1[1], BGR1[2], BGR1[2]);
		BGR1[2].convertTo(tmp1, CV_32F);
		cv::max(BGR2[0], BGR2[1], BGR2[1]);
		cv::max(BGR2[1], BGR2[2], BGR2[2]);
		BGR2[2].convertTo(tmp2, CV_32F);
		cv::subtract(tmp2, tmp1, tmp);
		return tmp;
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
						Mat diff = getDiffInVchannelHSV(frame, next, 0);
						correlations[i] += abs(cv::mean(diff).val[0]);
						
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
	static string createOuputVideoName(string inputMessage,int symbol_time,string inputVideoFile,string outputVideoFile)
	{
		ostringstream outputVideoStream;
		outputVideoStream << inputMessage << "_" << FREQ[ZERO] << "Hz_" << FREQ[ONE] << "Hz_";
		outputVideoStream << symbol_time << "ms_" << (LUMINANCE[0] - LUMINANCE[1]) << "levels_" << codec << "_" << inputVideoFile << "_";// << outputVideoFile;
		string str = outputVideoStream.str();
		std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
		str.erase(end_pos, str.end());
		end_pos = std::remove(str.begin(), str.end(), '.');
		str.erase(end_pos, str.end());
		end_pos = std::remove(str.begin(), str.end(), '\\');
		str.erase(end_pos, str.end());
		return str + outputVideoFile;
	}

	

	/// get video frames luminance
	// VideoCapture as input
	// ROI as input
	// returns vector<float> with the luminances
	static vector<vector<float> > getVideoFrameLuminances(VideoCapture cap, vector<cv::Rect> ROIs, double fps, bool useChessBoard, cv::Rect globalROI, bool oldMethod = false, bool useAlpha = true)
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
			nextIndex += fps / ReceiveParameters::framesPerSymbol;;
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
				cv::resize(frame, temp, cv::Size(640,480));
				if (canDetectMyBoard(frame))
				{
					break;
				}
			}

			Mat add_mask = MaskFactory::getBackgroundMask(prev(globalROI), frame(globalROI));// prev_mask & mask;
			Mat tmp_frame;
			frame(globalROI).copyTo(tmp_frame, add_mask);
			Mat tmp_prev;
			prev(globalROI).copyTo(tmp_prev, add_mask);
			for (int i = 0; i < ROIs.size(); i++)
			{
				Mat tmp;
				if (oldMethod)
				{
					if (useAlpha)
					{
						tmp = Utilities::getIntensity(tmp_frame(ROIs[i]));
					}
					else
					{
						tmp = Utilities::getVchannel(tmp_frame(ROIs[i]));
					}
					add_mask = add_mask * 0 + 255;
				}
				else
				{
					tmp = Utilities::getDiffInVchannelHSV(tmp_prev(ROIs[i]), tmp_frame(ROIs[i]), 0);
				}
				float luminance = cv::sum(tmp).val[0];
				float summation = (cv::sum(add_mask(ROIs[i])).val[0] / 255.0) + 1;
				luminance /= summation;
				//float luminance = getLuminance(tmp, ROI);
				if (abs(luminance) < 0.001 && frames[i].size())
				{
					frames[i].push_back(frames[i][frames[i].size() - 1]);
				}
				else
				{
					frames[i].push_back(luminance);
				}
			}
			prev = frame.clone();
		}
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
			nextIndex += fps / ReceiveParameters::framesPerSymbol;;
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
				Mat tmp = Utilities::getDiffInVchannelHSV(tmp_prev(ROIs[i]), tmp_frame(ROIs[i]), 0);
				float luminance = cv::sum(tmp).val[0];
				float summation = (cv::sum(add_mask(ROIs[i])).val[0] / 255.0) + 1;
				luminance /= summation;
				//float luminance = getLuminance(tmp, ROI);
				if (abs(luminance) < 0.001 && frames[i].size())
				{
					frames[i].push_back(frames[i][frames[i].size() - 1]);
				}
				else
				{
					frames[i].push_back(luminance);
				}
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
	static vector<cv::Rect> getDivisions(int divisions,double percent,bool cropInclusive,cv::Rect globalROI,bool translateToOriginal)
	{
		int frame_width, frame_height;
		frame_width = globalROI.width;
		frame_height = globalROI.height;
		int sectionsPerWidth = sqrt(divisions);
		int sectionsPerHeight = sectionsPerWidth;
		vector<cv::Rect> ROIs;
		int sectionWidth = 0, sectionHeight = 0;
		if (cropInclusive)
		{
			if (divisions <= 2)
			{
				sectionWidth = frame_width / divisions;
				sectionHeight = frame_height;
				sectionsPerWidth = divisions;
				sectionsPerHeight = 1;
			}
			else
			{
				sectionWidth = frame_width / sectionsPerWidth;
				sectionHeight = frame_height / sectionsPerHeight;
			}
			for (int y = 0; y < sectionsPerHeight; y++)
			{
				for (int x = 0; x < sectionsPerWidth; x++)
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
			if (divisions <= 2)
			{
				sectionWidth = (frame_width * percent) / divisions;
				sectionHeight = frame_height * percent;
				sectionsPerWidth = divisions;
				sectionsPerHeight = 1;
			}
			else
			{
				sectionWidth = (frame_width * percent) / sectionsPerWidth;
				sectionHeight = (frame_height * percent) / sectionsPerHeight;
			}			
			int widthStart = frame_width * (1 - percent);
			int heightStart = frame_height * (1 - percent);
			for (int y = 0; y < sectionsPerHeight; y++)
			{
				for (int x = 0; x < sectionsPerWidth; x++)
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
		return ROIs;
	}

	// this function is trying to find athe chess board and detect the last frame with the chess board and to detect the global ROI as well
	static cv::Rect getGlobalROI(string videoName, int &starting_index)
	{
		cv::Rect globalROI(0, 0, 1, 1);
		VideoCapture cap(videoName); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
			return globalROI;
		double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
		int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
		int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
		cap.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
		// try to detect the chess board
		Mat frame;
		globalROI.width = frame_width;
		globalROI.height = frame_height;
		starting_index = 0;
		for (; cap.read(frame); starting_index++)
		{
			cv::resize(frame, frame, cv::Size(640, 480));
			// this loop to detect the first chess board
			//cout << index << endl;
			//imshow("frame", frame);
			//cv::waitKey(0);
			if (canDetectMyBoard(frame))
			{
				break;
			}
		}
		int countChess = 0;
		int countErrors = 0;
		for (; cap.read(frame) && countErrors < 10; starting_index++, countChess++)
		{
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
		starting_index -= (countErrors + 1);
		cap.release();
		return globalROI;
	}
	/// get video frames luminance (this is the split version which splits the image into two)
	// video name as input
	// percentage of the frame as input (used to get this percentage from the center of the image) and takes value from (0,1]
	// int &framerate: is output parameter
	// divisions: supports 2 and 4 only for now
	// ROI: is the area of interest only for the whole image
	static vector<vector<float> > getVideoFrameLuminancesSplitted(string videoName, double percent, int &framerate, int divisions,bool useGlobalROI,
		cv::Scalar color = cv::Scalar(-1,-1,-1))
	{
		vector<vector<float> > frames;
		VideoCapture cap(videoName); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
			return frames;
		framerate = cap.get(CV_CAP_PROP_FPS); //get the frame rate
		// try to detect the chess board
		ReceiveParameters::globalROI = cv::Rect(0, 0, cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));
		if (useGlobalROI)
		{
			ReceiveParameters::globalROI = getGlobalROI(videoName, ReceiveParameters::startingIndex);
			cap.set(CV_CAP_PROP_POS_FRAMES, ReceiveParameters::startingIndex);
		}
		cout << "Index = " << ReceiveParameters::startingIndex << endl;
		// the ROI		
		vector<cv::Rect> ROIs = getDivisions(divisions, percent, false, ReceiveParameters::globalROI, false);
		if (color == cv::Scalar(-1, -1, -1))
		{
			frames = getVideoFrameLuminances(cap, ROIs, framerate, true, ReceiveParameters::globalROI);
		}
		else
		{
			frames = getVideoFrameLuminancesWithColorTracking(cap, ROIs, framerate, true, ReceiveParameters::globalROI, color);
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
			globalROI = getGlobalROI(videoName, index);
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
		return getVideoFrameLuminances(cap, ROIs, framerate, true, globalROI, true, useAlpha)[0];
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
		return Utilities::getVideoFrameLuminances(cap, ROIs, framerate, false, cv::Rect(0, 0, frame_width, frame_height))[0];
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
		return Utilities::getVideoFrameLuminances(cap, ROI, framerate, false, cv::Rect(0, 0, frame_width, frame_height))[0];
	}

	// open a video writer to use the same codec with every one
	static VideoWriter getVideoWriter(string vidName,double framerate, cv::Size frameSize)
	{
		// 0 -> no compression, DIVX, H264
		unsigned long milliseconds_since_epoch =
			std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::system_clock::now().time_since_epoch()).count();
		ostringstream name;
		name << milliseconds_since_epoch << "_" << vidName;
		VideoWriter vidWriter(name.str(),
			CV_FOURCC(codec[0], codec[1], codec[2], codec[3])
			, framerate, frameSize);
		return vidWriter;
	}
	static cv::Size getFrameSize()
	{
		return DefaultFrameSize;
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
				int err[] = { 16, 21, 22, 24, 25, 27, 29, 30, 31, 36, 45, 56, 70, 71, 76, 77, 78, 81, 82, 86, 129, 133, 134, 136, 137, 138, 139, 144, 145, 153, 168, 170, 178, 184, 185, 186, 189, 190, 193, 194 };
				for (int i = 0; i < 40; i++)
				{
					errors.push_back(err[i]);
				}
				for (int i = 0; i < errors.size(); i++)
				{
					ostringstream outputName;
					outputName << std::setfill('0') << std::setw(10);
					int start_data = ReceiveParameters::framesPerSymbol * (errors[i] + 1);
					int end_data = start_data + ReceiveParameters::framesPerSymbol;
					int start_second = framerate * (errors[i] + 1);
					int end_second = start_second + framerate;
					outputName << start_second / framerate << "_" << end_second / framerate << "_" << orig_msg[errors[i]] << "_" << test_msg[errors[i]];
					VideoWriter vidWriter;
					string outVideo = outputName.str() + ".avi";
					string outFrameValues = outputName.str() + ".frames";
					//cout << "ROI: (" << ReceiveParameters::globalROI.x << "," << ReceiveParameters::globalROI.y << ")\t(";
					//cout << ReceiveParameters::globalROI.width << "," << ReceiveParameters::globalROI.height << ")\n";
					videoReader.set(CV_CAP_PROP_POS_FRAMES, start_second + ReceiveParameters::startingIndex);
					vidWriter.open(outVideo, CV_FOURCC('D', 'I', 'V', 'X'), framerate, cv::Size(ReceiveParameters::globalROI.width, ReceiveParameters::globalROI.height));
					for (int j = 0; j <= framerate; j++)
					{
						videoReader >> frame;
						vidWriter << frame(ReceiveParameters::globalROI);
					}
					ofstream outFrames(outFrameValues);
					for (int j = start_data; j <= end_data; j++)
					{
						outFrames << ReceiveParameters::amplitudes[j] << endl;
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
		int xStep = (board.cols * boarderPercentage) / (patternsize.width + 1);
		int yStep = (board.rows * boarderPercentage) / (patternsize.height + 1);
		int xStart = (board.cols * (1 - boarderPercentage)) / 2;
		int yStart = (board.rows * (1 - boarderPercentage)) / 2;
		for (int x = 0; x <= patternsize.width; x += 2)
		{
			for (int y = 0; y <= patternsize.height; y += 2)
			{
				cv::rectangle(board,
					cv::Point(xStart + x*xStep, yStart + y*yStep),
					cv::Point(xStart + (x + 1)*xStep, yStart + (y + 1)*yStep),
					cv::Scalar(0, 0, 0),
					CV_FILLED);

			}
		}
		for (int x = 1; x <= patternsize.width; x += 2)
		{
			for (int y = 1; y <= patternsize.height; y += 2)
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
	// img: input image in BGR
	// patternSize: interior number of corners
	// return rectangle around the calibration points
	static cv::Rect detectMyBoard(Mat img)
	{
		// create the gray image
		Mat gray; //source image
		if (img.channels() == 3)
		{
			cv::cvtColor(img, gray, CV_BGR2GRAY);
		}
		else
		{
			gray = img.clone();
		}
		vector<Point2f> corners; //this will be filled by the detected corners

		//CALIB_CB_FAST_CHECK saves a lot of time on images
		//that do not contain any chessboard corners
		bool patternfound = findChessboardCorners(gray, patternsize, corners,
			CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE
			+ CALIB_CB_FAST_CHECK);

		cv::Rect result;
		if (patternfound)
		{
			cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
				TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, ReceiveParameters::framesPerSymbol, 0.1));
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
		result.x = xl;
		result.y = yl;
		result.width = xh - xl + 1;
		result.height = yh - yl + 1;
		return result;
	}


	// img: input image in BGR
	// return true if board detected
	static bool canDetectMyBoard(Mat img, cv::Size myPattern = patternsize)
	{
		// create the gray image
		Mat gray; //source image
		if (img.channels() == 3)
		{
			cv::cvtColor(img, gray, CV_BGR2GRAY);
		}
		else
		{
			gray = img.clone();
		}
		vector<Point2f> corners; //this will be filled by the detected corners

		//CALIB_CB_FAST_CHECK saves a lot of time on images
		//that do not contain any chessboard corners
		return findChessboardCorners(gray, myPattern, corners,
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
};
