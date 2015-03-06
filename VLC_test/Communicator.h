#pragma once

#include "Utilities.h"
#include "WaveGenerator.h"

class Communicator
{
protected:
	// display the given image in the given window name for the given millisecionds
	//void displayImageForCertainPeriod(Mat img, string winName, int displayTime)
	//{
	//	typedef std::chrono::high_resolution_clock Clock;
	//	typedef std::chrono::milliseconds milliseconds;
	//	Clock::time_point t0 = Clock::now();
	//	// start work
	//	imshow(winName, img);
	//	// end work
	//	Clock::time_point t1 = Clock::now();
	//	milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	//	// wait for some time 
	//	cvWaitKey(max(1, (int)(displayTime - ms.count())));
	//}

	/// generate sequence of 24 frame that should be displayed with the given frequency in the given region of interest
	/// frequency is in Hz
	/// int bitTime in millisecond
	//void diplay24FramesWithSomeFrequency(Rect ROI, double frequency, Mat& new_image1, Mat& new_image2, int bitTime)
	//{
	//	//printf("%d", (int)frequency);
	//	// screen frequency
	//	double screen_refresh_rate = 60; // 60Hz
	//	int duty_cycle = (int)ceil(1000 / frequency); // seconds
	//	if (duty_cycle & 1)
	//	{
	//		// make it even
	//		++duty_cycle;
	//	}
	//	double time_for_the_high_amplitude = duty_cycle / 2; // in milli seconds
	//	// the time needed for one bit is bittime milliseconds
	//	int number_of_iterations = bitTime / (time_for_the_high_amplitude * 2);

	//	for (int i = 0; i < number_of_iterations; i++)
	//	{
	//		displayImageForCertainPeriod(new_image1, "video", time_for_the_high_amplitude);
	//		displayImageForCertainPeriod(new_image2, "video", duty_cycle - time_for_the_high_amplitude);
	//	}
	//}

	/// generate sequence of 24 frame that should be displayed with the given frequency in the given region of interest
	/// frequency is in Hz
	/// int bitTime in millisecond
	/// number of frames for each cycle
	//void addFramesWithSomeFrequency(VideoWriter & vidWriter, Rect ROI, double frequency, Mat& new_image1, Mat& new_image2, int bitTime, int frames_cycle)
	//{
	//	//double screen_refresh_rate = 60; // 60Hz
	//	double duty_cycle = 1000 / frequency; // seconds
	//	// the time needed for one bit is 400 milliseconds
	//	int number_of_iterations = bitTime / duty_cycle;

	//	for (int i = 0; i < number_of_iterations; i++)
	//	{
	//		for (int j = 0; j < frames_cycle / 2; j++)
	//		{
	//			vidWriter << new_image1;
	//			//cout << 1;
	//		}
	//		for (int j = 0; j < frames_cycle / 2; j++)
	//		{
	//			vidWriter << new_image2;
	//			//cout << 0;
	//		}
	//	}
	//}

	//void sendMessage(string imagefile, string msg)
	//{
	//	Mat new_image1;
	//	Mat new_image2;
	//	Mat image = imread(imagefile, IMREAD_COLOR); // Read the file
	//	Rect ROI = Rect(0, 0, image.cols, image.rows);
	//	new_image1 = image.clone();
	//	Utilities::updateFrameWithAlpha(new_image1, ROI, LUMINANCE[1]);
	//	new_image2 = image.clone();
	//	Utilities::updateFrameWithAlpha(new_image2, ROI, LUMINANCE[0]);

	//	// convert the message into sequence of bits

	//	for (int i = 0; i < msg.length(); i++)
	//	{
	//		char c = msg[i];
	//		char x = 0;

	//		for (int j = 0; j < 8; j++)
	//		{
	//			x = (x << 1) | ((c >> (7 - j)) & 1);
	//			typedef std::chrono::high_resolution_clock Clock;
	//			typedef std::chrono::milliseconds milliseconds;
	//			Clock::time_point t0 = Clock::now();
	//			// work done here
	//			diplay24FramesWithSomeFrequency(ROI, FREQ[(c >> (7 - j)) & 1], new_image1, new_image2, 1000);
	//			// end work
	//			Clock::time_point t1 = Clock::now();
	//			milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	//			std::cout << ms.count() << "ms\n";
	//		}
	//		printf("%c", x);
	//	}
	//}

	// symbol_time: how many milliseconds will the symbol last
	//void createOfflineVideo(string imagefile, string msg, string outputVideoFile, int symbol_time, bool createVid = true)
	//{
	//	if (createVid)
	//	{
	//		Mat new_image1;
	//		Mat new_image2;
	//		Mat image = imread(imagefile, IMREAD_COLOR); // Read the file
	//		Rect ROI = Rect(0, 0, image.cols, image.rows);
	//		new_image1 = image.clone();
	//		Utilities::updateFrameWithAlpha(new_image1, ROI, LUMINANCE[1]);
	//		new_image2 = image.clone();
	//		Utilities::updateFrameWithAlpha(new_image2, ROI, LUMINANCE[0]);


	//		// convert the message into sequence of bits

	//		// get greatest common divisor for both fps
	//		int gcdFreq = gcd((int)FREQ[ZERO], (int)FREQ[ONE]);
	//		int lcmFreq = FREQ[ZERO] * FREQ[ONE] / gcdFreq;
	//		VideoWriter vidWriter;
	//		vidWriter.open(outputVideoFile, CV_FOURCC('D', 'I', 'V', 'X'), lcmFreq * 2, image.size());
	//		for (int i = 0; i < msg.length(); i++)
	//		{
	//			char c = msg[i];
	//			char x = 0;

	//			for (int j = 7; j >= 0; j--)
	//			{
	//				x = (x << 1) | ((c >> (7 - j)) & 1);
	//				typedef std::chrono::high_resolution_clock Clock;
	//				typedef std::chrono::milliseconds milliseconds;
	//				Clock::time_point t0 = Clock::now();
	//				// work done here
	//				addFramesWithSomeFrequency(vidWriter, ROI, FREQ[(c >> (7 - j)) & 1], new_image1, new_image2, symbol_time, lcmFreq * 2 / FREQ[(c >> (7 - j)) & 1]);
	//				//for (int k = 0;k < )
	//				// end work
	//				Clock::time_point t1 = Clock::now();
	//				milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	//				//std::cout << ms.count() << "ms\n";
	//				cout << (int)((c >> (7 - j)) & 1);
	//			}
	//			//printf("%c", x);
	//		}
	//	}
	//}
protected:
	int frame_width;
	int frame_height;
	//double fps;
	double inputFrameUsageFrames; // used for videos
	Mat img;
	//int symbol_time;
	cv::Rect globalROI;
	vector<vector<float> > amplitudes;
	vector<cv::Rect> ROIs;
	vector<short> shortMsg;
	vector<SymbolData> msg;
	VideoCapture videoReader;
	VideoWriter vidWriter;
public:

	bool initImage(string inputImage, vector<short> msg, string outputVideoFile)
	{
		this->shortMsg = msg;
		this->msg = Parameters::symbolsData.getMsgSymbols(msg);
		img = imread(inputImage);
		cv::resize(img, img, Utilities::getFrameSize());
		//imshow("img", img);
		//cv::waitKey(0);
		frame_width = img.cols;
		frame_height = img.rows;
		vidWriter = Utilities::getVideoWriter(getVideoName(outputVideoFile), Utilities::getFrameSize());
		globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
		return true;
	}
	bool initVideo(string inputVideoFile, vector<short> msg, string outputVideoFile)
	{
		videoReader.open(inputVideoFile);
		if (videoReader.isOpened())
		{
			this->shortMsg = msg;
			this->msg = Parameters::symbolsData.getMsgSymbols(msg);
			videoReader.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			double framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
			frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
			frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
			Parameters::fps = Utilities::getOuputVideoFrameRate((int)framerate);

			inputFrameUsageFrames = Parameters::fps / framerate;

			vidWriter = Utilities::getVideoWriter(getVideoName(outputVideoFile), Utilities::getFrameSize());
			globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
			return true;
		}
		return false;
	}
	void addSynchFrames(bool end)
	{
		if (end)
		{
			Utilities::addDummyFramesToVideo(vidWriter, Parameters::fps);
			Utilities::addDummyFramesToVideo(vidWriter, Parameters::fps, Utilities::createChessBoard());
			vidWriter.release();
		}
		else
		{
			Utilities::addDummyFramesToVideo(vidWriter, Parameters::fps, Utilities::createChessBoard());
			Utilities::addDummyFramesToVideo(vidWriter, Parameters::fps);
		}
	}

	virtual string getVideoName(string outputVideoFile)
	{
		return outputVideoFile;
	}
	virtual void initCommunication()
	{
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(msg));
	}
	virtual void sendImageMainLoop()
	{
		for (int i = 0; i < amplitudes[0].size(); i++)
		{
			//cout << i << endl;
			Mat frame = img.clone();
			//cv::resize(img, frame, Utilities::getFrameSize());
			Utilities::updateFrameLuminance(frame, globalROI, amplitudes[0][i]);
			//frame.convertTo(frame, CV_32F);
			vidWriter << frame;
		}
	}
	virtual vector<Mat> getSplittedImages(Mat &frame)
	{
		return vector<Mat>();
	}
	virtual void sendVideoMainLoop()
	{
		Mat frame;
		double frameIndex = 0;
		for (int k = 0; k < amplitudes[0].size(); k++)
		{
			if (k >= frameIndex)
			{
				frameIndex += inputFrameUsageFrames;
				videoReader.read(frame);
				//cout << (amplitudes[k]) << endl;
			}
			Mat tmp;
			cv::resize(frame, tmp, Utilities::getFrameSize());
			Utilities::updateFrameLuminance(tmp, globalROI, amplitudes[0][k]);
			vidWriter << tmp;
		}
	}
	// symbol_time: how many milliseconds will the symbol last
	void sendImage()
	{
		initCommunication();
		addSynchFrames(false);
		sendImageMainLoop();
		addSynchFrames(true);
	}

	// symbol_time: how many milliseconds will the symbol last
	void sendVideo()
	{
		if (videoReader.isOpened())
		{
			initCommunication();
			addSynchFrames(false);
			sendVideoMainLoop();
			addSynchFrames(true);
		}
	}

	
	vector<short> receive2(vector<float> frames, int fps)
	{
		/*for (int i = 0; i < frames.size(); i++)
		{
			printf("%f\n", frames[i]);
		}
		*/
		int frames_per_symbol = fps * Parameters::symbolTime / 1000;
		if (Parameters::DecodingMethod == CROSS_CORRELATION)
		{
			return receiveCrossCorrelation(frames, fps, frames_per_symbol);
		}
		return receiveFFT(frames, fps, frames_per_symbol);
	}
	// calculate the best fit between two signals based on cross-correlation and return the peek value
	vector<double> calcCrossCorrelate(vector< vector<float> > &signals, vector<float> &test,int start, int end)
	{
		//double totalBestVal = 0;
		vector<double> bestVal(signals.size(), 0);
		vector<double> avgAmplitude(signals.size(), 0);
		vector<int> count(signals.size(), 0);
		int tsz = end - start + 1;
		int ssz = signals[0].size();
		for (int i = -(tsz >> 1); i < (ssz >> 1);i++)
		{	
			vector<double> sum(signals.size(), 0);
			vector<double> sumAmp(signals.size(), 0);
			vector<int> cntAmp(signals.size(), 0);
			//double totalSum = 0;
			for (int j = std::max(0, i); j < std::min(ssz, tsz + i); j++)
			{
				for (int k = 0; k < signals.size(); k++)
				{
					sum[k] += signals[k][j] * test[j - i + start];
					/*if (test[j - i + start] > test[j - i + start - 1] && test[j - i + start] > test[j - i + start + 1])
					{
						sumAmp[k] += abs(test[j - i + start]);
						cntAmp[k]++;
					}
					else if (test[j - i + start] < test[j - i + start - 1] && test[j - i + start] < test[j - i + start + 1])
					{
						sumAmp[k] += abs(test[j - i + start]);
						cntAmp[k]++;
					}*/
				}
			}
			//sum /= cnt;
			//cout << i << "\t" << sum << endl;
			for (int k = 0; k < sum.size(); k++)
			{
				if (sum[k] > bestVal[k])
				{
					bestVal[k] = sum[k];
					//avgAmplitude[k] = sumAmp[k] / cntAmp[k];
				}
			}
		}
		int maxID = 0;
		for (int i = 1; i < bestVal.size(); i++)
		{
			if(bestVal[i] > bestVal[maxID])
			{
				maxID = i;
			}
		}
		//cout << avgAmplitude[maxID] << "\t";
		
		//cout << endl;
		return bestVal;
	}

	// receive using cross-correlation as classifier
	vector<short> receiveCrossCorrelation(vector<float> frames, int fps, int frames_per_symbol)
	{
		// return array
		vector<short> result;
		// create the signals to use in correlation
		vector<vector<vector<float> > > signals;
		//signals.push_back(WaveGenerator::createSampledSineWave(fps, frames_per_symbol, Parameters::FREQ[0]));
		//signals.push_back(WaveGenerator::createSampledSineWave(fps, frames_per_symbol, Parameters::FREQ[1]));
		for (int i = 0; i < Parameters::symbolsData.allData.size(); i++)
		{
			vector<vector<float> > signal;
			for (double phase = 0; phase < MM_PI; phase += MM_PI / 4)
			{
				//signals.push_back(WaveGenerator::createSampledSquareWave(fps, frames_per_symbol, Parameters::symbolsData.allData[i].frequency, 1, -1));
				signal.push_back(WaveGenerator::createSampledSineWave(fps, frames_per_symbol, Parameters::symbolsData.allData[i].frequency, phase));
				//signals.push_back(WaveGenerator::createSampledSineWave(fps, frames_per_symbol,
				//	Parameters::symbolsData.allData[i].frequency, Parameters::symbolsData.allData[i].frequency, -Parameters::symbolsData.allData[i].frequency));
			}
			signals.push_back(signal);
		}
		int window_size = frames_per_symbol;
		int end = frames.size() - fps;
		for (int i = fps; i < end; i += window_size)
		{
			if (Parameters::symbolsData.allData.size() == 2)
			{
				// normalize signal
				for (int j = 0; j < window_size; j++)
				{
					if (frames[j + i] > 0)
					{
						frames[j + i] = 1;
					}
					else if (frames[j + i] < 0)
					{
						frames[j + i] = -1;
					}
				}
			}
			vector<double> Detected;
			for (int j = 0; j < signals.size(); j++)
			{
				vector<double> bestOfSignal = calcCrossCorrelate(signals[j], frames, i, i + window_size);
				Detected.push_back(*max_element(bestOfSignal.begin(), bestOfSignal.end()));
			}
			// get maximum response
			int maxIdx = 0;
			for (int j = 1; j < signals.size(); j++)
			{
				if (Detected[j] > Detected[maxIdx])
				{
					maxIdx = j;
				}
			}
			vector<short> maxSymbol = Parameters::symbolsData.allData[maxIdx].getSymbol();
			result.insert(result.end(), maxSymbol.begin(), maxSymbol.end());
			//if (Detected[1] > Detected[0])
			//{
			//	//cout << oneDetected << endl;
			//	result.push_back(1);
			//}
			//else// if (zeroDetected > oneDetected)
			//{
			//	//cout << zeroDetected << endl;
			//	result.push_back(0);
			//}
			////cout << endl;
			////else
			////{
			////	result.push_back(2);
			////}
		}
		return result;
	}
	vector<short> receiveFFT(vector<float> frames, int fps, int frames_per_symbol)
	{
		Parameters::amplitudes = frames;
		vector<short> result;
		vector<vector<int> > detection(Parameters::symbolsData.allData.size(), vector<int>(frames.size(), 0));
		//vector<int> zero_detected(frames.size(), 0);
		//vector<int> one_detected(frames.size(), 0);
		vector<int> other_detected(frames.size(), 0);
		int window_size = frames_per_symbol;
		for (int i = 0; i < frames.size() - fps; i++)
		{
			//cout << frames[i] << endl;
			vector<Frequency> temp = Utilities::myft(frames, fps, i, window_size);
			// get the maximum frequency for this set of frames
			int maxi = 0;
			for (int j = 1; j < temp.size(); j++)
			{
				if (temp[j].percent > temp[maxi].percent)
				{
					maxi = j;
				}
			}
			bool foundFreq = false;
			for (int j = 0; j < Parameters::symbolsData.allData.size(); j++)
			{
				if (abs(temp[maxi].freq - Parameters::symbolsData.allData[j].frequency) < EPSILON)
				{
					for (int k = 0; k < window_size; k++)
					{
						detection[j][i + k]++;
					}
					foundFreq = true;
					break;
				}
			}
			if (!foundFreq)
			{
				for (int j = 0; j < window_size; j++) other_detected[i + j]++;
			}
			//if (abs(temp[maxi].freq - Parameters::FREQ[0]) < EPSILON)
			//{
			//	// ZERO detectd
			//	for (int j = 0; j < window_size; j++) zero_detected[i + j]++;
			//}
			//else if (abs(temp[maxi].freq - Parameters::FREQ[1]) < EPSILON)
			//{
			//	// one detected
			//	for (int j = 0; j < window_size; j++) one_detected[i + j]++;
			//}
			//else
			//{
			//	// other detected
			//	for (int j = 0; j < window_size; j++) other_detected[i + j]++;
			//}
			
		}
		// then check for the first frame that has 60% or more with one of the two symbols (0,1), 
		// and the symbol should have enough time (at least after the first FRAMES_PER_SYMBOL have been passed)
		int starting_index = fps + frames_per_symbol / 2; // to be in the middle of the first symbol as we have the first symbol time empty
		//for (int i = frames_per_symbol; i < frames.size() - frames_per_symbol; i++)
		//{
		//	if ((zero_detected[i] + one_detected[i]) * 10 >= (zero_detected[i] + one_detected[i] + other_detected[i]) * 6)
		//	{
		//		// this first frame
		//		starting_index = i;
		//		break;
		//	}
		//}
		cout << "Starting Index Inside loaded frames = " << starting_index << endl;
		// for the rest of the symbols
		// just follow the same rule
		for (int i = starting_index; i < frames.size() - fps; i += frames_per_symbol)
		{
			//if (zero_detected[i] > one_detected[i])
			//{
			//	// this first frame and zero
			//	result.push_back(0);
			//}
			//else if (one_detected[i] > zero_detected[i])
			//{
			//	// this first frame and one
			//	result.push_back(1);
			//}
			int maxIdx = 0;
			for (int j = 1; j < Parameters::symbolsData.allData.size(); j++)
			{
				if (detection[j][i] > detection[maxIdx][i])
				{
					maxIdx = j;
				}
			}
			bool foundSymbol = true;
			for (int j = 1; j < Parameters::symbolsData.allData.size(); j++)
			{
				if (j != maxIdx && detection[maxIdx][i] > detection[j][i])
				{
					foundSymbol = false;
					break;
				}
			}
			if (foundSymbol)
			{
				vector<short> maxSymbol = Parameters::symbolsData.allData[maxIdx].getSymbol();
				result.insert(result.end(), maxSymbol.begin(), maxSymbol.end());
			}
			else
			{
				if (Parameters::DecodingMethod == FFT_NO_RANDOM_GUESS)
				{
					for (int j = 0; j < Parameters::symbolsData.allData[0].symbol.size(); j++)
					{
						result.push_back(2);
					}
				}
				else if (Parameters::DecodingMethod == FFT_RANDOM_GUESS)
				{
					for (int j = 0; j < Parameters::symbolsData.allData[0].symbol.size(); j++)
					{
						if (result.size() == 0)
						{
							result.push_back(1);
						}
						else
						{
							result.push_back((~result[result.size() - 1]) & 1);
						}
					}
				}
				
			}
		}
		return result;
	}

	// receive with a certain ROI ratio
	virtual vector<short> receive(string fileName, double ROI_Ratio)
	{
		int fps = 0;
		vector<float> frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps,1,1,true,false)[0];
		return receive2(frames, fps);
	}

	// receive with a certain ROI ratio
	virtual vector<short> receiveColor(string fileName, double ROI_Ratio, cv::Scalar color)
	{
		puts("color");
		int fps = 0;
		vector<float> frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps, 1,1, true, false,color)[0];
		return receive2(frames, fps);
	}

	// receive with a certain ROI ratio
	void receiveWithSelectionByHand(string fileName)
	{
		int fps = 0;
		vector<float> frames = Utilities::getVideoFrameLuminances(fileName, fps);
		receive2(frames, fps);
	}

};