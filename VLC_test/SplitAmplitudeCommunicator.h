#pragma once
#include "Communicator.h"
class SplitAmplitudeCommunicator :
	public Communicator
{
public:
	////////////////////////////// Split Amplitude ///////////////////////////
	virtual string getVideoName(string outputVideoFile)
	{
		return "_AmpDiff_" + outputVideoFile;
	}
	virtual void initCommunication()
	{
		//double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(msg, Parameters::LUMINANCE));
		//double lumin2[] = { LUMINANCE[1], LUMINANCE[0] };
		amplitudes.push_back(WaveGenerator::createWaveGivenFPS(msg, -Parameters::LUMINANCE));

		ROIs = Utilities::getDivisions(2, 1, false, globalROI, true);
	}
	virtual void sendImageMainLoop()
	{
		for (int i = 0; i < amplitudes[0].size(); i++)
		{
			Mat frame = img.clone();
			Utilities::updateFrameLuminance(frame, ROIs[0], amplitudes[0][i]);
			Utilities::updateFrameLuminance(frame, ROIs[1], amplitudes[1][i]);
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
			vidWriter << tmp;
		}
	}
	//// symbol_time: how many milliseconds will the symbol last
	//void sendImage()
	//{
	//	//Mat img = imread(inputImage);
	//	//cv::resize(img, img, Utilities::getFrameSize());
	//	//int framerate = frequency; //get the frame rate
	//	//int frame_width = img.cols;
	//	//int frame_height = img.rows;
	//	//int frames_per_symbol = (framerate * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
	//	double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
	//	vector<float> amplitudes1 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1);
	//	double lumin2[] = { LUMINANCE[1], LUMINANCE[0] };
	//	vector<float> amplitudes2 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin2);
	//	// create the video writer
	//	//ostringstream outputVideoStream;
	//	//outputVideoStream << msg.size() << "_AmpDiff" << Utilities::createOuputVideoName(symbol_time, "image", outputVideoFile);
	//	//VideoWriter vidWriter = Utilities::getVideoWriter("_AmpDiff_" + outputVideoFile, fps, Utilities::getFrameSize());
	//	//cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
	//	ROIs = Utilities::getDivisions(2, 1, false, globalROI,true);
	//	addSynchFrames(false);
	//	for (int i = 0; i < amplitudes1.size();i++)
	//	{
	//		Mat frame = img.clone();
	//		Utilities::updateFrameLuminance(frame, ROIs[0], amplitudes1[i]);
	//		Utilities::updateFrameLuminance(frame, ROIs[1], amplitudes2[i]);
	//		vidWriter << frame;
	//	}
	//	addSynchFrames(true);
	//}

	// symbol_time: how many milliseconds will the symbol last
	//void sendVideo()
	//{
	//	//VideoCapture videoReader(inputVideoFile);
	//	if (videoReader.isOpened())
	//	{
	//		//videoReader.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
	//		//int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
	//		//int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
	//		//int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
	//		//while (frame_height > 1000)
	//		//{
	//		//	frame_width /= 2;
	//		//	frame_height /= 2;
	//		//}
	//		//int fps = Utilities::getOuputVideoFrameRate((int)framerate);
	//		//int frames_per_symbol = (fps * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
	//		double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
	//		vector<float> amplitudes1 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1);
	//		double lumin2[] = { LUMINANCE[1], LUMINANCE[0] };
	//		vector<float> amplitudes2 = WaveGenerator::createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin2);
	//		Mat frame;
	//		
	//		// create the video writer
	//		//ostringstream outputVideoStream;
	//		//outputVideoStream << msg.size() << "_AmpDiff" << Utilities::createOuputVideoName(symbol_time, inputVideoFile, outputVideoFile);
	//		//VideoWriter vidWriter = Utilities::getVideoWriter("_AmpDiff_" + outputVideoFile, fps, Utilities::getFrameSize());
	//		//int inputFrameUsageFrames = fps / framerate;
	//		videoReader.read(frame);
	//		//cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
	//		ROIs = Utilities::getDivisions(2, 1, false, globalROI, true);
	//		addSynchFrames(false);
	//		for (int k = 0; k < amplitudes1.size(); k++)
	//		{
	//			if (k%inputFrameUsageFrames == 0)
	//			{
	//				videoReader.read(frame);
	//			}
	//			Mat tmp;
	//			cv::resize(frame, tmp, Utilities::getFrameSize());
	//			Utilities::updateFrameLuminance(tmp, ROIs[0], amplitudes1[k]);
	//			Utilities::updateFrameLuminance(tmp, ROIs[1], amplitudes2[k]);
	//			vidWriter << tmp;
	//		}
	//		addSynchFrames(true);
	//	}
	//	cout << endl;
	//}

	/////////////////////////////////////////////////////////////////////////////////////////////
	///              //////////////      Receive     ///////////////                         ////
	/////////////////////////////////////////////////////////////////////////////////////////////


	// receive with a certain ROI ratio
	vector<short> receive(string fileName, double ROI_Ratio)
	{
		int fps = 0;
		vector<vector<float> > frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps, 2,true);
		vector<float> amplitude_difference;
		//double avg = 0;
		for (int i = 0; i < frames[0].size(); i++)
		{
			amplitude_difference.push_back(frames[0][i] - frames[1][i]);
			//avg = std::max((double)abs(amplitude_difference[i]),avg);
		}
		//cout << "Avg amplitude = " << avg << endl;
		return receive2(amplitude_difference, fps);
	}
};

