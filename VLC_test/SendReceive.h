#pragma once

#include "SelectByMouse.h"
#include "Utilities.h"


class SendReceive
{
public:
	// display the given image in the given window name for the given millisecionds
	static void displayImageForCertainPeriod(Mat img, string winName, int displayTime)
	{
		typedef std::chrono::high_resolution_clock Clock;
		typedef std::chrono::milliseconds milliseconds;
		Clock::time_point t0 = Clock::now();
		// start work
		imshow(winName, img);
		// end work
		Clock::time_point t1 = Clock::now();
		milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
		// wait for some time 
		cvWaitKey(max(1, (int)(displayTime - ms.count())));
	}

	/// generate sequence of 24 frame that should be displayed with the given frequency in the given region of interest
	/// frequency is in Hz
	/// int bitTime in millisecond
	static void diplay24FramesWithSomeFrequency(Rect ROI, double frequency, Mat& new_image1, Mat& new_image2, int bitTime)
	{
		//printf("%d", (int)frequency);
		// screen frequency
		double screen_refresh_rate = 60; // 60Hz
		int duty_cycle = (int)ceil(1000 / frequency); // seconds
		if (duty_cycle & 1)
		{
			// make it even
			++duty_cycle;
		}
		double time_for_the_high_amplitude = duty_cycle / 2; // in milli seconds
		// the time needed for one bit is bittime milliseconds
		int number_of_iterations = bitTime / (time_for_the_high_amplitude * 2);

		for (int i = 0; i < number_of_iterations; i++)
		{
			displayImageForCertainPeriod(new_image1, "video", time_for_the_high_amplitude);
			displayImageForCertainPeriod(new_image2, "video", duty_cycle - time_for_the_high_amplitude);
		}
	}

	/// generate sequence of 24 frame that should be displayed with the given frequency in the given region of interest
	/// frequency is in Hz
	/// int bitTime in millisecond
	/// number of frames for each cycle
	static void addFramesWithSomeFrequency(VideoWriter & vidWriter, Rect ROI, double frequency, Mat& new_image1, Mat& new_image2, int bitTime, int frames_cycle)
	{
		//double screen_refresh_rate = 60; // 60Hz
		double duty_cycle = 1000 / frequency; // seconds
		// the time needed for one bit is 400 milliseconds
		int number_of_iterations = bitTime / duty_cycle;

		for (int i = 0; i < number_of_iterations; i++)
		{
			for (int j = 0; j < frames_cycle / 2; j++)
			{
				vidWriter << new_image1;
				//cout << 1;
			}
			for (int j = 0; j < frames_cycle / 2; j++)
			{
				vidWriter << new_image2;
				//cout << 0;
			}
		}
	}

	static void sendMessage(string imagefile, string msg)
	{
		Mat new_image1;
		Mat new_image2;
		Mat image = imread(imagefile, IMREAD_COLOR); // Read the file
		Rect ROI = Rect(0, 0, image.cols, image.rows);
		new_image1 = image.clone();
		Utilities::updateFrameWithAlpha(new_image1, ROI, LUMINANCE[1]);
		new_image2 = image.clone();
		Utilities::updateFrameWithAlpha(new_image2, ROI, LUMINANCE[0]);

		// convert the message into sequence of bits

		for (int i = 0; i < msg.length(); i++)
		{
			char c = msg[i];
			char x = 0;

			for (int j = 0; j < 8; j++)
			{
				x = (x << 1) | ((c >> (7 - j)) & 1);
				typedef std::chrono::high_resolution_clock Clock;
				typedef std::chrono::milliseconds milliseconds;
				Clock::time_point t0 = Clock::now();
				// work done here
				diplay24FramesWithSomeFrequency(ROI, FREQ[(c >> (7 - j)) & 1], new_image1, new_image2, 1000);
				// end work
				Clock::time_point t1 = Clock::now();
				milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
				std::cout << ms.count() << "ms\n";
			}
			printf("%c", x);
		}
	}

	

	// symbol_time: how many milliseconds will the symbol last
	static void createOfflineVideo(string imagefile, string msg, string outputVideoFile, int symbol_time, bool createVid = true)
	{
		if (createVid)
		{
			Mat new_image1;
			Mat new_image2;
			Mat image = imread(imagefile, IMREAD_COLOR); // Read the file
			Rect ROI = Rect(0, 0, image.cols, image.rows);
			new_image1 = image.clone();
			Utilities::updateFrameWithAlpha(new_image1, ROI, LUMINANCE[1]);
			new_image2 = image.clone();
			Utilities::updateFrameWithAlpha(new_image2, ROI, LUMINANCE[0]);


			// convert the message into sequence of bits

			// get greatest common divisor for both fps
			int gcdFreq = gcd((int)FREQ[ZERO], (int)FREQ[ONE]);
			int lcmFreq = FREQ[ZERO] * FREQ[ONE] / gcdFreq;
			VideoWriter vidWriter;
			vidWriter.open(outputVideoFile, CV_FOURCC('D', 'I', 'V', 'X'), lcmFreq * 2, image.size());
			for (int i = 0; i < msg.length(); i++)
			{
				char c = msg[i];
				char x = 0;

				for (int j = 7; j >= 0; j--)
				{
					x = (x << 1) | ((c >> (7 - j)) & 1);
					typedef std::chrono::high_resolution_clock Clock;
					typedef std::chrono::milliseconds milliseconds;
					Clock::time_point t0 = Clock::now();
					// work done here
					addFramesWithSomeFrequency(vidWriter, ROI, FREQ[(c >> (7 - j)) & 1], new_image1, new_image2, symbol_time, lcmFreq * 2 / FREQ[(c >> (7 - j)) & 1]);
					//for (int k = 0;k < )
					// end work
					Clock::time_point t1 = Clock::now();
					milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
					//std::cout << ms.count() << "ms\n";
					cout << (int)((c >> (7 - j)) & 1);
				}
				//printf("%c", x);
			}
		}
	}

	// symbol_time: how many milliseconds will the symbol last
	static void createOfflineVideoWithGivenFPS(double frequency, string inputImage, string msg, string outputVideoFile, int symbol_time)
	{
		Mat img = imread(inputImage);

		int framerate = frequency; //get the frame rate
		int frame_width = img.cols;
		int frame_height = img.rows;
		//int frames_per_symbol = (framerate * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
		int lumin[] = { LUMINANCE[0], LUMINANCE[1] };
		vector<int> amplitudes = createWaveGivenFPS(frequency, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin);
		// create the video writer
		VideoWriter vidWriter;
		vidWriter.open(outputVideoFile, CV_FOURCC('D', 'I', 'V', 'X'), framerate, cv::Size(frame_width, frame_height));
		for (int i = 0; i < amplitudes.size(); i++)
		{
			Mat frame = img.clone();
			Utilities::updateFrameWithAlpha(frame, cv::Rect(0, 0, frame.cols, frame.rows), amplitudes[i]);
			vidWriter << frame;
		}
	}

	// symbol_time: how many milliseconds will the symbol last
	static vector<int> createWaveGivenFPS(double frequency, string msg, int symbol_time,int ZeroFrequency,int OneFrequency,int luminance[2])
	{
		vector<int> amplitudes;
		int framerate = frequency; //get the frame rate
		int frames_per_symbol = (framerate * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
		int frequencies[] = { ZeroFrequency, OneFrequency };
		// create the video writer
		VideoWriter vidWriter;
		for (int i = 0; i < msg.length(); i++)
		{
			for (int j = 7; j >= 0; j--)
			{
				int needed_frequency = frequencies[(msg[i] >> (7 - j)) & 1];
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
				cout << (int)((msg[i] >> (7 - j)) & 1);
			}
		}
		cout << endl;
		return amplitudes;
	}

	// symbol_time: how many milliseconds will the symbol last
	static void createOfflineVideoFromVideo(string inputVideoFile, string msg, string outputVideoFile, int symbol_time)
	{
		VideoCapture videoReader(inputVideoFile);
		if (videoReader.isOpened())
		{
			videoReader.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
			int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
			int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
			int fps = Utilities::lcm((int)framerate, Utilities::lcm(2 * FREQ[ZERO], 2 * FREQ[ONE]));
			int frames_per_symbol = (fps * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
			Mat frame;
			// create the video writer
			VideoWriter vidWriter;
			vidWriter.open(outputVideoFile, CV_FOURCC('D', 'I', 'V', 'X'), fps, cv::Size(frame_width, frame_height));
			int inputFrameUsageFrames = fps / framerate;
			int InputFrameCounter = 0;
			for (int i = 0; i < msg.length(); i++)
			{
				for (int j = 7; j >= 0; j--)
				{
					int needed_frequency = FREQ[(msg[i] >> (7 - j)) & 1];
					int frames_per_half_cycle = fps / (needed_frequency * 2);
					// start high
					int luminance_index = 0;
					for (int k = 0; k < frames_per_symbol; k++, InputFrameCounter++)
					{
						if ((InputFrameCounter%inputFrameUsageFrames) == 0)
						{
							if (!videoReader.read(frame))
							{
								InputFrameCounter--;
								break;
							}
						}
						if ((k%frames_per_half_cycle) == 0)
						{
							luminance_index ^= 1;
						}
						Mat tmp = frame.clone();
						Utilities::updateFrameWithAlpha(tmp, cv::Rect(0, 0, frame.cols, frame.rows), LUMINANCE[luminance_index]);
						vidWriter << tmp;
					}
					cout << (int)((msg[i] >> (7 - j)) & 1);
				}
			}

		}
		cout << endl;
	}

	static float getLuminanceWithMaskFromGray(Mat& frame, cv::Rect& ROI)
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

	static float getLuminanceWithMaskFromHSV(Mat& frame, cv::Rect& ROI, double percent)
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

	static float getLuminanceFromGray(Mat& frame, cv::Rect& ROI)
	{
		// get the ROI
		Mat tmp = frame(ROI);
		// convert the color for the ROI only
		cv::cvtColor(tmp, tmp, CV_BGR2GRAY);
		cv::Scalar tempVal = mean(tmp);
		float myMAtMean = tempVal.val[0];

		return myMAtMean;
	}

	static float getLuminanceFromHSV(Mat& frame, cv::Rect& ROI)
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

	static float getLuminance(Mat& frame, cv::Rect& ROI)
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
		return getVideoFrameLuminances(cap, ROI);
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
		return getVideoFrameLuminances(cap, ROI);
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

	// 
	static int getFirstFrameIndex(vector<float> frame_luminance, int frames_per_symbol, int start_index = 0)
	{
		vector<Frequency> temp;
		for (int i = 0; frame_luminance.size() - start_index - i >= frames_per_symbol; i++)
		{
			temp = Utilities::myft(frame_luminance, 30, i + start_index, frames_per_symbol);
			//cout << "test " << i << endl;
			// test the needed two frequencies for 60% correct
			int maxi = 0;
			for (int j = 1; j < temp.size(); j++)
			{
				if (temp[j].percent > temp[maxi].percent)
				{
					maxi = j;
				}
			}
			if (temp[maxi].percent > 0.25)
			{
				if (abs(temp[maxi].freq - FREQ[ZERO]) < EPSILON)
				{
					//cout << start_index + i << "\tFound Zero" << endl;
					printf("0");
					return start_index + i;
				}
				if (abs(temp[maxi].freq - FREQ[ONE]) < EPSILON)
				{
					//cout << start_index + i << "\tFound One" << endl;
					printf("1");
					return start_index + i;
				}
			}
		}
		return -1;
	}


	static int getNextFrameIndex(vector<float> frame_luminance, int frames_per_symbol, int start_index)
	{
		vector<Frequency> temp;
		for (int i = 0; frame_luminance.size() - start_index - i >= frames_per_symbol; i++)
		{
			temp = Utilities::myft(frame_luminance, 30, i + start_index, frames_per_symbol);
			//cout << "test " << i << endl;
			// test the needed two frequencies for 60% correct
			int ZEROIndex = 0;
			int ONEIndex = 0;
			for (int j = 1; j < temp.size(); j++)
			{
				if (abs(temp[j].freq - FREQ[ZERO]) < abs(temp[ZEROIndex].freq - FREQ[ZERO]))
				{
					ZEROIndex = j;
				}
				if (abs(temp[j].freq - FREQ[ONE]) < abs(temp[ONEIndex].freq - FREQ[ONE]))
				{
					ONEIndex = j;
				}
			}
			if (temp[ZEROIndex].percent > temp[ONEIndex].percent)
			{
				//cout << start_index + i << "\tFound Zero" << endl;
				printf("0");
				return start_index + i;
			}
			else if (temp[ZEROIndex].percent < temp[ONEIndex].percent)
			{
				//cout << start_index + i << "\tFound One" << endl;
				printf("1");
				return start_index + i;
			}
		}
		return -1;
	}

	static void send()
	{
		string fileName = "D:\\MSECE_IUPUI\\MSECE_IUPUI\\Testing_image\\img2.jpg";
		string msg = "";// "This is a test message, I am trying to send bits";
		char a = 0xec;
		for (int i = 0; i < 5; i++)
		{
			msg += a;
		}
		sendMessage(fileName, msg);
		//createOfflineVideo(fileName, msg, "output.avi",1000);
	}

	// msg: will be converted to binary
	static void send(string msg, string fileName = "D:\\MSECE_IUPUI\\MSECE_IUPUI\\Testing_image\\img2.jpg")
	{
		createOfflineVideo(fileName, msg, "output.avi", 1000);
	}
	static void receive(string fileName)
	{
		vector<float> frames;
		int fps;
		frames = getVideoFrameLuminances(fileName, 1, fps);
		//int count = 0;
		//cout << ++count << "\t";
		int frames_per_symbol = 30;
		int index = getFirstFrameIndex(frames, frames_per_symbol, 0);
		for (; index > 0 && index < frames.size() - frames_per_symbol;)
		{
			//cout << ++count << "\t";
			index = getNextFrameIndex(frames, frames_per_symbol, index + frames_per_symbol);
		}
		puts("");
		//myft(frames,30,180);
		//myft();
	}
	static void receive2(vector<float> frames, int fps, int frames_per_symbol)
	{
		vector<int> zero_detected(frames.size(), 0);
		vector<int> one_detected(frames.size(), 0);
		vector<int> other_detected(frames.size(), 0);
		for (int i = 0; i < frames.size() - frames_per_symbol; i++)
		{
			vector<Frequency> temp = Utilities::myft(frames, fps, i, frames_per_symbol);
			// get the maximum frequency for this set of frames
			int maxi = 0;
			for (int j = 1; j < temp.size(); j++)
			{
				if (temp[j].percent > temp[maxi].percent)
				{
					maxi = j;
				}
			}
			if (abs(temp[maxi].freq - FREQ[ZERO]) < EPSILON)
			{
				// ZERO detectd
				for (int j = 0; j < frames_per_symbol; j++) zero_detected[i + j]++;
			}
			else if (abs(temp[maxi].freq - FREQ[ONE]) < EPSILON)
			{
				// one detected
				for (int j = 0; j < frames_per_symbol; j++) one_detected[i + j]++;
			}
			else
			{
				// other detected
				for (int j = 0; j < frames_per_symbol; j++) other_detected[i + j]++;
			}
		}
		// then check for the first frame that has 60% or more with one of the two symbols (0,1), 
		// and the symbol should have enough time (at least after the first FRAMES_PER_SYMBOL have been passed)
		int starting_index = 0;
		for (int i = frames_per_symbol; i < frames.size() - frames_per_symbol; i++)
		{
			if (zero_detected[i] * 10 >= (zero_detected[i] + one_detected[i] + other_detected[i]) * 6)
			{
				// this first frame and zero
				starting_index = i;
				printf("0");
				break;
			}
			else if (one_detected[i] * 10 >= (zero_detected[i] + one_detected[i] + other_detected[i]) * 6)
			{
				// this first frame and one
				starting_index = i;
				printf("1");
				break;
			}
		}
		// for the rest of the symbols
		// just follow the same rule
		for (int i = starting_index + frames_per_symbol; i < frames.size() - frames_per_symbol; i += frames_per_symbol)
		{
			if (zero_detected[i] > one_detected[i])
			{
				// this first frame and zero
				printf("0");
			}
			else if (one_detected[i] > zero_detected[i])
			{
				// this first frame and one
				starting_index = i;
				printf("1");
			}
			else
			{
				i = (i + 1 - frames_per_symbol);
			}
		}
	}

	// receive with a certain ROI ratio
	static void receiveWithInputROIRatio(string fileName, int frames_per_symbol, double ROI_Ratio)
	{
		int fps = 0;
		vector<float> frames = getVideoFrameLuminances(fileName, ROI_Ratio, fps);
		receive2(frames, fps, frames_per_symbol);
	}

	// receive with a certain ROI ratio
	static void receiveWithInputROIRectangle(string fileName, int frames_per_symbol)
	{
		int fps = 0;
		vector<float> frames = getVideoFrameLuminances(fileName, fps);
		receive2(frames, fps, frames_per_symbol);
	}

	/// get video frames luminance
	static void extractAllVideoFrames(string videoName)
	{
		VideoCapture cap(videoName); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
			return;
		double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
		cout << count << endl;
		//Mat edges;
		//namedWindow("edges", 1);
		cout << "Processing Frames..." << endl;
		Mat frame;
		cap.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
		int ind = 0;
		while (1)
		{
			Mat frame;
			//cap >> frame;
			bool success = cap.read(frame);
			ind++;
			if (!success){
				cout << "End frame processing." << endl;
				break;
			}
			if (ind > 8000 && !(ind % 50))
			{
				ostringstream ostr;
				ostr << std::setfill('0') << std::setw(10);
				ostr << ind << ".jpg";
				imwrite(ostr.str(), frame);
			}
		}
	}
};
