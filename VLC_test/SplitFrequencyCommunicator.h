#pragma once
#include "Communicator.h"
class SplitFrequencyCommunicator :
	public Communicator
{
public:
	///////////////////////////// Split Frequency /////////////////////////////////////////

	// symbol_time: how many milliseconds will the symbol last
	void sendImage(double frequency, string inputImage, string msg, string outputVideoFile, int symbol_time)
	{
		Mat img = imread(inputImage);

		int framerate = frequency; //get the frame rate
		int frame_width = img.cols;
		int frame_height = img.rows;
		//int frames_per_symbol = (framerate * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
		double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
		vector<float> amplitudes1 = createWaveGivenFPS(frequency, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1);
		vector<float> amplitudes2 = createWaveGivenFPS(frequency, msg, symbol_time, FREQ[ONE], FREQ[ZERO], lumin1);
		// create the video writer
		ostringstream outputVideoStream;
		outputVideoStream << msg << Utilities::createOuputVideoName(symbol_time, "image", outputVideoFile);
		VideoWriter vidWriter = Utilities::getVideoWriter(outputVideoStream.str(), framerate, cv::Size(frame_width, frame_height));
		for (int i = 0; i < amplitudes1.size(); i++)
		{
			Mat frame = img.clone();
			Utilities::updateFrameWithAlpha(frame, cv::Rect(0, 0, frame.cols / 2, frame.rows), amplitudes1[i]);
			Utilities::updateFrameWithAlpha(frame, cv::Rect(frame.cols / 2, 0, frame.cols / 2, frame.rows), amplitudes2[i]);
			vidWriter << frame;
		}
	}

	// symbol_time: how many milliseconds will the symbol last
	void sendVideo(string inputVideoFile, string msg, string outputVideoFile, int symbol_time)
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
				frame_width /= 2;
				frame_height /= 2;
			}
			int fps = Utilities::lcm((int)framerate, Utilities::lcm(2 * FREQ[ZERO], 2 * FREQ[ONE]));
			//int frames_per_symbol = (fps * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
			double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
			vector<float> amplitudes1 = createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1);
			vector<float> amplitudes2 = createWaveGivenFPS(fps, msg, symbol_time, FREQ[ONE], FREQ[ZERO], lumin1);
			Mat frame;
			// create the video writer
			ostringstream outputVideoStream;
			outputVideoStream << msg << Utilities::createOuputVideoName(symbol_time, inputVideoFile, outputVideoFile);
			VideoWriter vidWriter = Utilities::getVideoWriter(outputVideoStream.str(), fps, cv::Size(frame_width, frame_height));
			int inputFrameUsageFrames = fps / framerate;
			for (int k = 0; k < amplitudes1.size(); k++)
			{
				if (k%inputFrameUsageFrames == 0)
				{
					videoReader.read(frame);
				}
				Mat tmp;
				cv::resize(frame, tmp, cv::Size(frame_width, frame_height));
				Utilities::updateFrameWithAlpha(tmp, cv::Rect(0, 0, tmp.cols / 2, tmp.rows), amplitudes1[k]);
				Utilities::updateFrameWithAlpha(tmp, cv::Rect(tmp.cols / 2, 0, tmp.cols / 2, tmp.rows), amplitudes2[k]);
				vidWriter << tmp;

			}
		}
		cout << endl;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	///              //////////////      Receive     ///////////////                         ////
	/////////////////////////////////////////////////////////////////////////////////////////////

	// receive with a certain ROI ratio
	void receive(string fileName, int frames_per_symbol, double ROI_Ratio)
	{
		int fps = 0;
		vector<vector<float> > frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps, 2);
		receiveWithInputROIRatioFreqDiff(frames, fps, frames_per_symbol);
	}

protected:

	void receiveWithInputROIRatioFreqDiff(vector<vector<float> > frames, int fps, int frames_per_symbol)
	{
		double sum = 0;
		vector<float> maxFreq1, maxFreq2, freqDiff;
		vector<float> freqDiffAvg;
		for (int i = 0; i < frames[0].size() - frames_per_symbol; i++)
		{
			vector<Frequency> temp0 = Utilities::myft(frames[0], fps, i, frames_per_symbol);
			vector<Frequency> temp1 = Utilities::myft(frames[1], fps, i, frames_per_symbol);
			// get the difference between maximums
			int max0 = 0, max1 = 0;
			for (int j = 1; j < temp0.size(); j++)
			{
				if (temp0[j].percent > temp0[max0].percent)
				{
					max0 = j;
				}
				if (temp1[j].percent > temp1[max1].percent)
				{
					max1 = j;
				}
			}
			maxFreq1.push_back(temp0[max0].freq);
			maxFreq2.push_back(temp1[max1].freq);
			freqDiff.push_back(temp0[max0].freq - temp1[max1].freq);
			sum += freqDiff[i];
			if (i >= frames_per_symbol)
			{
				sum -= freqDiff[i - frames_per_symbol];
				freqDiffAvg.push_back(sum / frames_per_symbol);
			}
		}
		// output the numbers
		int ind = 0;
		for (; ind < freqDiff.size(); ind++)
		{
			//cout << endl << ind << "\t" << maxFreq1[ind] << "\t" << maxFreq2[ind] << "\t" << freqDiff[ind] << "\t";
			if (abs(freqDiff[ind] - (FREQ[ZERO] - FREQ[ONE])) < EPSILON)
			{
				printf("0");
				break;
			}
			else if (abs(freqDiff[ind] - (FREQ[ONE] - FREQ[ZERO])) < EPSILON)
			{
				printf("1");
				break;
			}
		}
		int i = 0;
		do
		{
			i++, ind++;
			//cout << endl << ind << "\t" << maxFreq1[ind] << "\t" << maxFreq2[ind] << "\t" << freqDiff[ind] << "\t";
		} while (i < frames_per_symbol && ind < freqDiff.size() - 1);
		for (; ind < freqDiff.size() - 1;)
		{
			if (abs(freqDiff[ind] - (FREQ[ZERO] - FREQ[ONE])) < EPSILON)
			{
				printf("0");
				i = 0;
				do
				{
					i++, ind++;
					//cout << endl << ind << "\t" << maxFreq1[ind] << "\t" << maxFreq2[ind] << "\t" << freqDiff[ind] << "\t";
				} while (i < frames_per_symbol && ind < freqDiff.size() - 1);
			}
			else if (abs(freqDiff[ind] - (FREQ[ONE] - FREQ[ZERO])) < EPSILON)
			{
				printf("1");
				i = 0;
				do
				{
					i++, ind++;
					//cout << endl << ind << "\t" << maxFreq1[ind] << "\t" << maxFreq2[ind] << "\t" << freqDiff[ind] << "\t";
				} while (i < frames_per_symbol && ind < freqDiff.size() - 1);
			}
			else
			{
				ind++;
				//cout << endl << ind << "\t" << maxFreq1[ind] << "\t" << maxFreq2[ind] << "\t" << freqDiff[ind] << "\t";
			}
		}
	}

};

