#pragma once

#include "Utilities.h"

class Communicator
{
protected:
	// display the given image in the given window name for the given millisecionds
	void displayImageForCertainPeriod(Mat img, string winName, int displayTime)
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
	void diplay24FramesWithSomeFrequency(Rect ROI, double frequency, Mat& new_image1, Mat& new_image2, int bitTime)
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
	void addFramesWithSomeFrequency(VideoWriter & vidWriter, Rect ROI, double frequency, Mat& new_image1, Mat& new_image2, int bitTime, int frames_cycle)
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

	void sendMessage(string imagefile, string msg)
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
	void createOfflineVideo(string imagefile, string msg, string outputVideoFile, int symbol_time, bool createVid = true)
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
	vector<float> createWaveGivenFPS(double frequency, vector<short> msg, int symbol_time, int ZeroFrequency, int OneFrequency, double luminance[2])
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

public:
	// symbol_time: how many milliseconds will the symbol last
	virtual void sendImage(double frequency, string inputImage, vector<short> msg, string outputVideoFile, int symbol_time)
	{
		Mat img = imread(inputImage);

		int framerate = frequency; //get the frame rate
		int frame_width = img.cols;
		int frame_height = img.rows;
		//int frames_per_symbol = (framerate * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
		double lumin[] = { LUMINANCE[0], LUMINANCE[1] };
		vector<float> amplitudes = createWaveGivenFPS(frequency, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin);
		// create the video writer
		ostringstream outputVideoStream;
		outputVideoStream << msg.size() << Utilities::createOuputVideoName(symbol_time, "image", outputVideoFile);
		VideoWriter vidWriter = Utilities::getVideoWriter(outputVideoStream.str(), framerate, Utilities::getFrameSize());
		Utilities::addDummyFramesToVideo(vidWriter, framerate, Utilities::createChessBoard());
		Utilities::addDummyFramesToVideo(vidWriter, framerate);
		cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
		for (int i = 0; i < amplitudes.size(); i++)
		{
			Mat frame;
			cv::resize(img, frame, Utilities::getFrameSize());
			Utilities::updateFrameWithAlpha(frame, globalROI, amplitudes[i]);
			//frame.convertTo(frame, CV_32F);
			vidWriter << frame;
		}
		Utilities::addDummyFramesToVideo(vidWriter, framerate);
		Utilities::addDummyFramesToVideo(vidWriter, framerate, Utilities::createChessBoard());
	}

	// 
	int getFirstFrameIndex(vector<float> frame_luminance, int frames_per_symbol, int start_index = 0)
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


	int getNextFrameIndex(vector<float> frame_luminance, int frames_per_symbol, int start_index)
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

	void send()
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
	void send(string msg, string fileName = "D:\\MSECE_IUPUI\\MSECE_IUPUI\\Testing_image\\img2.jpg")
	{
		createOfflineVideo(fileName, msg, "output.avi", 1000);
	}


	// symbol_time: how many milliseconds will the symbol last
	virtual void sendVideo(string inputVideoFile, vector<short> msg, string outputVideoFile, int symbol_time)
	{
		VideoCapture videoReader(inputVideoFile);
		if (videoReader.isOpened())
		{
			videoReader.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
			int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
			int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
			while (frame_height > 1000)
			{
				frame_height >>= 1;
				frame_width >>= 1;
			}
			int fps = Utilities::lcm((int)framerate, Utilities::lcm(2 * FREQ[ZERO], 2 * FREQ[ONE]));
			int frames_per_symbol = (fps * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
			Mat frame;
			// create the video writer
			ostringstream outputVideoStream;
			outputVideoStream << msg.size() << Utilities::createOuputVideoName(symbol_time, inputVideoFile, outputVideoFile);
			VideoWriter vidWriter = Utilities::getVideoWriter(outputVideoStream.str(), fps, Utilities::getFrameSize());
			int inputFrameUsageFrames = fps / framerate;
			double lumin[] = { LUMINANCE[0], LUMINANCE[1] };
			vector<float> amplitudes = createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin);
			int InputFrameCounter = 0;
			Utilities::addDummyFramesToVideo(vidWriter, fps, Utilities::createChessBoard());
			Utilities::addDummyFramesToVideo(vidWriter, fps);
			cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
			for (int k = 0; k < amplitudes.size(); k++)
			{
				if (k%inputFrameUsageFrames == 0)
				{
					videoReader.read(frame);
					//cout << (amplitudes[k]) << endl;
				}
				Mat tmp;
				cv::resize(frame, tmp, Utilities::getFrameSize());
				Utilities::updateFrameWithAlpha(tmp, globalROI, amplitudes[k]);
				vidWriter << tmp;
			}
			Utilities::addDummyFramesToVideo(vidWriter, fps);
			Utilities::addDummyFramesToVideo(vidWriter, fps, Utilities::createChessBoard());
		}
		cout << endl;
	}

	void receive(string fileName)
	{
		vector<float> frames;
		int fps;
		frames = Utilities::getVideoFrameLuminances(fileName, 1, fps);
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
	vector<short> receive2(vector<float> frames, int fps, int frames_per_symbol)
	{
		vector<short> result;
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
			if ((zero_detected[i] + one_detected[i]) * 10 >= (zero_detected[i] + one_detected[i] + other_detected[i]) * 6)
			{
				// this first frame and zero
				starting_index = i;
				//result.push_back(0);
				break;
			}
			/*else if (one_detected[i] * 10 >= (zero_detected[i] + one_detected[i] + other_detected[i]) * 5)
			{
				// this first frame and one
				starting_index = i;
				result.push_back(1);
				break;
			}*/
		}
		// for the rest of the symbols
		// just follow the same rule
		for (int i = starting_index; i < frames.size() - frames_per_symbol; i += frames_per_symbol)
		{
			if (zero_detected[i] > one_detected[i])
			{
				// this first frame and zero
				result.push_back(0);
			}
			else// if (one_detected[i] > zero_detected[i])
			{
				// this first frame and one
				starting_index = i;
				result.push_back(1);
			}/*
			else
			{
				i = (i + 1 - frames_per_symbol);
			}*/
		}
		return result;
	}

	// receive with a certain ROI ratio
	virtual vector<short> receive(string fileName, int frames_per_symbol, double ROI_Ratio)
	{
		int fps = 0;
		vector<float> frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps,1,true)[0];
		return receive2(frames, 30, frames_per_symbol);
	}

	// receive with a certain ROI ratio
	void receiveWithSelectionByHand(string fileName, int frames_per_symbol)
	{
		int fps = 0;
		vector<float> frames = Utilities::getVideoFrameLuminances(fileName, fps);
		receive2(frames, fps, frames_per_symbol);
	}

	/// get video frames luminance
	void extractAllVideoFrames(string videoName)
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
