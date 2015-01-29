#pragma once

#include "Utilities.h"
#include "SplitFrequencyCommunicator.h"

class SplitFrequencyAmplitudeCommunicator : public SplitFrequencyCommunicator
{
public:
	
	
	////////////////////////////////// Split Frequency and Amplitude ////////////////////////////////
	virtual string getVideoName(string outputVideoFile)
	{
		return "_FreqAmpDiff_" + outputVideoFile;
	}

	virtual void initCommunication()
	{
		double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
		double lumin2[] = { LUMINANCE[1], LUMINANCE[0] };
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1));
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ONE], FREQ[ZERO], lumin1));
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin2));
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ONE], FREQ[ZERO], lumin2));
		
		ROIs = Utilities::getDivisions(4, 1, false, globalROI, true);
	}
	virtual void sendImageMainLoop()
	{
		for (int i = 0; i < amplitudes[0].size(); i++)
		{
			Mat frame = img.clone();
			//cv::resize(img, frame, Utilities::getFrameSize());
			Utilities::updateFrameLuminance(frame, ROIs[0], amplitudes[0][i]);
			Utilities::updateFrameLuminance(frame, ROIs[1], amplitudes[1][i]);
			Utilities::updateFrameLuminance(frame, ROIs[2], amplitudes[2][i]);
			Utilities::updateFrameLuminance(frame, ROIs[3], amplitudes[3][i]);
			vidWriter << frame;
		}
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
			}
			Mat tmp;
			cv::resize(frame, tmp, Utilities::getFrameSize());
			Utilities::updateFrameLuminance(tmp, ROIs[0], amplitudes[0][k]);
			Utilities::updateFrameLuminance(tmp, ROIs[1], amplitudes[1][k]);
			Utilities::updateFrameLuminance(tmp, ROIs[2], amplitudes[2][k]);
			Utilities::updateFrameLuminance(tmp, ROIs[3], amplitudes[3][k]);
			vidWriter << tmp;
		}
	}

	// symbol_time: how many milliseconds will the symbol last
	//void sendImage()
	//{
	//	//Mat img = imread(inputImage);

	//	//int fps = frequency; //get the frame rate
	//	//int frame_width = img.cols;
	//	//int frame_height = img.rows;
	//	//int frames_per_symbol = (framerate * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
	//	double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
	//	double lumin2[] = { LUMINANCE[1], LUMINANCE[0] };
	//	vector<float> amplitudes11 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1);
	//	vector<float> amplitudes12 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ONE], FREQ[ZERO], lumin1);
	//	vector<float> amplitudes21 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin2);
	//	vector<float> amplitudes22 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ONE], FREQ[ZERO], lumin2);
	//	// create the video writer
	//	//ostringstream outputVideoStream;
	//	//outputVideoStream << msg.size() << "_FreqAmpDiff" << Utilities::createOuputVideoName(symbol_time, "image", outputVideoFile);
	//	//VideoWriter vidWriter = Utilities::getVideoWriter("_FreqAmpDiff_" + outputVideoFile, fps, Utilities::getFrameSize());
	//	//cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
	//	ROIs = Utilities::getDivisions(4, 1, false, globalROI,true);
	//	addSynchFrames(false);
	//	for (int i = 0; i < amplitudes11.size(); i++)
	//	{
	//		Mat frame = img.clone();
	//		//cv::resize(img, frame, Utilities::getFrameSize());
	//		Utilities::updateFrameLuminance(frame, cv::Rect(0, 0, frame.cols / 2, frame.rows / 2), amplitudes11[i]);
	//		Utilities::updateFrameLuminance(frame, cv::Rect(frame.cols / 2, 0, frame.cols / 2, frame.rows / 2), amplitudes12[i]);
	//		Utilities::updateFrameLuminance(frame, cv::Rect(0, frame.rows / 2, frame.cols / 2, frame.rows / 2), amplitudes21[i]);
	//		Utilities::updateFrameLuminance(frame, cv::Rect(frame.cols / 2, frame.rows / 2, frame.cols / 2, frame.rows / 2), amplitudes22[i]);
	//		vidWriter << frame;
	//	}
	//	addSynchFrames(true);
	//}

	//void sendVideo(/*string inputVideoFile, vector<short> msg, string outputVideoFile, int symbol_time*/)
	//{
	//	//VideoCapture videoReader(inputVideoFile);
	//	if (videoReader.isOpened())
	//	{
	//		//videoReader.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
	//		//int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
	//		//int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
	//		//int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
	//		//int fps = Utilities::getOuputVideoFrameRate((int)framerate);
	//		//int frames_per_symbol = (fps * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
	//		double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
	//		double lumin2[] = { LUMINANCE[1], LUMINANCE[0] };
	//		vector<float> amplitudes11 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1);
	//		vector<float> amplitudes12 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ONE], FREQ[ZERO], lumin1);
	//		vector<float> amplitudes21 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin2);
	//		vector<float> amplitudes22 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ONE], FREQ[ZERO], lumin2);
	//		Mat frame;
	//		// create the video writer
	//		//ostringstream outputVideoStream;
	//		//outputVideoStream << msg.size() << "_FreqAmpDiff" << Utilities::createOuputVideoName(symbol_time, inputVideoFile, outputVideoFile);
	//		//VideoWriter vidWriter = Utilities::getVideoWriter("_FreqAmpDiff_" + outputVideoFile, fps, Utilities::getFrameSize());
	//		//int inputFrameUsageFrames = fps / framerate;
	//		videoReader.read(frame);
	//		//cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
	//		ROIs = Utilities::getDivisions(4, 1, false, globalROI,true);
	//		addSynchFrames(false);
	//		for (int k = 0; k < amplitudes11.size(); k++)
	//		{
	//			if (k%inputFrameUsageFrames == 0)
	//			{
	//				videoReader.read(frame);
	//			}
	//			Mat tmp;
	//			cv::resize(frame, tmp, Utilities::getFrameSize());
	//			Utilities::updateFrameLuminance(tmp, ROIs[0], amplitudes11[k]);
	//			Utilities::updateFrameLuminance(tmp, ROIs[1], amplitudes12[k]);
	//			Utilities::updateFrameLuminance(tmp, ROIs[2], amplitudes21[k]);
	//			Utilities::updateFrameLuminance(tmp, ROIs[3], amplitudes22[k]);
	//			vidWriter << tmp;
	//		}
	//		addSynchFrames(true);
	//	}
	//	cout << endl;
	//}

	// receive with a certain ROI ratio
	vector<short> receive(string fileName, int frames_per_symbol, double ROI_Ratio)
	{
		int fps = 0;
		vector<vector<float> > temp = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps, 4,true);
		vector<vector<float> > frames(2);
		for (int i = 0; i < temp[0].size(); i++)
		{
			frames[0].push_back(temp[0][i] - temp[2][i]);
			frames[1].push_back(temp[1][i] - temp[3][i]);
		}
		return receiveWithInputROIRatioFreqDiff(frames, 30,frames_per_symbol);
	}
};
