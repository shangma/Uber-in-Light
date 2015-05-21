/*
Copyright (c) 2015, mostafa izz
izz.mostafa@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the name of MyVLC nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "Utilities.h"
#include "WaveGenerator.h"

class Communicator
{
private:
	VideoWriter vidWriter;
protected:
	
	double inputFrameUsageFrames; // used for videos
	Mat img;
	vector<vector<float> > amplitudes;
	vector<cv::Rect> ROIs;
	vector<short> shortMsg;
	vector<SymbolData> msg;
	VideoCapture videoReader;
	void writeFrame(Mat &frame)
	{
		Utilities::writeFrame(vidWriter,frame);
	}
public:
	void setCommonParameters(vector<short> &msg, string outputVideoFile)
	{
		this->shortMsg = msg;
		this->msg = Parameters::symbolsData.getMsgSymbols(msg);
		vidWriter = Utilities::getVideoWriter(getVideoName(outputVideoFile), Utilities::getFrameSize());
		switch (Parameters::synchMethod)
		{
		case SYNCH_CHESS:
			Parameters::globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
			break;
		case SYNCH_GREEN_CHANNEL:
			Parameters::globalROI = cv::Rect(0, 0, Parameters::DefaultFrameSize.width, Parameters::DefaultFrameSize.height);
			break;
		}
	}
	bool initImage(string inputImage, vector<short> &msg, string outputVideoFile)
	{
		
		img = imread(inputImage);
		cv::resize(img, img, Utilities::getFrameSize());
		//imshow("img", img);
		//cv::waitKey(0);
		//frame_width = img.cols;
		//frame_height = img.rows;
		
		setCommonParameters(msg,outputVideoFile);
		return true;
	}
	bool initVideo(string inputVideoFile, vector<short> msg, string outputVideoFile)
	{
		videoReader.open(inputVideoFile);
		if (videoReader.isOpened())
		{
			videoReader.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			double framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
			//frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
			//frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
			Parameters::fps = Utilities::getOuputVideoFrameRate((int)framerate);

			inputFrameUsageFrames = Parameters::fps / framerate;

			setCommonParameters(msg, outputVideoFile);
			return true;
		}
		return false;
	}
	void addSynchFrames(bool end)
	{
		switch (Parameters::synchMethod)
		{
		case SYNCH_GREEN_CHANNEL:
		{
			double frameIndex = 0;
			
			vector<float> wave(Parameters::fps / 2, 0);
			vector<float> tmp = WaveGenerator::createSampledSquareWave(Parameters::fps, Parameters::fps / 2, 12, 0.008, -0.008);
			wave.insert(wave.end(), tmp.begin(), tmp.end());
			wave.resize(Parameters::fps * 3 / 2, 0);
			tmp = WaveGenerator::createSampledSquareWave(Parameters::fps, Parameters::fps / 2, 9, 0.008, -0.008);
			wave.insert(wave.end(), tmp.begin(), tmp.end());
			wave.push_back(0);
			int i = 0;
			for (int j = 0; j < wave.size(); j++, i++)
			{
				if (Parameters::realVideo)
				{
					if (i >= frameIndex)
					{
						frameIndex += inputFrameUsageFrames;
						videoReader.read(img);
						cv::resize(img, img, Utilities::getFrameSize());
					}
				}
				vector<Mat> BGR;
				cv::split(img, BGR);

				Utilities::updateFrameLuminance(BGR[0], Parameters::globalROI, -wave[j]);
				Utilities::updateFrameLuminance(BGR[1], Parameters::globalROI, wave[j]);
				Utilities::updateFrameLuminance(BGR[2], Parameters::globalROI, -wave[j]);

				Mat frame;
				cv::merge(BGR, frame);
				writeFrame(frame);
			}
		}
		break;
		case SYNCH_CHESS:
			if (end)
			{
				//Utilities::addDummyFramesToVideo(vidWriter, Parameters::fps);
				Utilities::addDummyFramesToVideo(vidWriter, Parameters::fps, Utilities::createChessBoard());
				//vidWriter.release();
			}
			else
			{
				Utilities::addDummyFramesToVideo(vidWriter, Parameters::fps, Utilities::createChessBoard());
				Utilities::addDummyFramesToVideo(vidWriter, Parameters::fps);
			}
			break;
		}
		if (end)
		{
			Parameters::done = true;
			vidWriter.release();
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
			Utilities::updateFrameLuminance(frame, Parameters::globalROI, amplitudes[0][i]);
			//frame.convertTo(frame, CV_32F);
			writeFrame(frame);
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
			Utilities::updateFrameLuminance(tmp, Parameters::globalROI, amplitudes[0][k]);
			writeFrame(tmp);
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
			double phase = 0;
			for (int j = 0; j < 5; j++, phase += MM_PI / 4)
			{
				signal.push_back(WaveGenerator::createSampledSineWave(fps, frames_per_symbol, Parameters::symbolsData.allData[i].frequency, phase));
			}
			signals.push_back(signal);
		}
		int window_size = frames_per_symbol;
		int end = frames.size() - ((Parameters::synchMethod == SYNCH_CHESS) ? fps : 0);
		int start = (Parameters::synchMethod == SYNCH_CHESS) ? fps : 0;
		vector<int> best_start(signals.size(), 0);
		vector<int> best_end(signals.size(), 0);
		vector<int> test_start(signals.size(), 0);
		for (int i = start; i < end; i += window_size)
		{
			vector<double> Detected;
			for (int j = 0; j < signals.size(); j++)
			{
				vector<double> bestOfSignal = Utilities::calcCrossCorrelate(signals[j], frames, i, i + window_size, best_start, best_end, test_start);
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