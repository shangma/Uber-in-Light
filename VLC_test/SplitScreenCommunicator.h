#pragma once
#include "Communicator.h"

class SplitScreenCommunicator :
	public Communicator
{
public:
	////////////////////////////// Split to quarters ///////////////////////////
	int sectionsPerLength = 4;	// number of splitting sections
	vector<cv::Rect> ROIs;
	SplitScreenCommunicator(int cellsPerSide)
	{
		sectionsPerLength = cellsPerSide;
	}
	// symbol_time: how many milliseconds will the symbol last
	void sendImage(double frequency, string inputImage, vector<short> msg, string outputVideoFile, int symbol_time)
	{
		int sections = sectionsPerLength * sectionsPerLength;
		Mat img = imread(inputImage);
		cv::resize(img, img, Utilities::getFrameSize());
		int framerate = frequency; //get the frame rate
		int frame_width = img.cols;
		int frame_height = img.rows;
		//int frames_per_symbol = (framerate * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
		double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
		vector<float> amplitudes1 = createWaveGivenFPS(frequency, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1);
		int framesForSymbol = (frequency * 1000) / symbol_time;
		/*int sectionsPerLength = sqrt(sections);
		int sectionWidth = Utilities::getFrameSize().width / sectionsPerLength;
		int sectionHeight = Utilities::getFrameSize().height / sectionsPerLength;*/
		// create the video writer
		//ostringstream outputVideoStream;
		//outputVideoStream << msg.size() << "_Split" << sections << Utilities::createOuputVideoName(symbol_time, "image", outputVideoFile);
		VideoWriter vidWriter = Utilities::getVideoWriter("_Split_" + outputVideoFile, framerate, Utilities::getFrameSize());
		// get the sections
		cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
		ROIs = Utilities::getDivisions(sections, frame_width, frame_height, 1, false, globalROI);
		// add dummy seconds in the beginning of the video
		Utilities::addDummyFramesToVideo(vidWriter, framerate, Utilities::createChessBoard());
		Utilities::addDummyFramesToVideo(vidWriter, framerate);
		for (int i = 0; i < amplitudes1.size();i += (sections * framesForSymbol))
		{
			for (int k = 0; k < framesForSymbol; k++)
			{
				Mat frame = img.clone();
				for (int j = 0; j < sections; j++)
				{
					// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
					Utilities::updateFrameWithAlpha(frame, ROIs[j], amplitudes1[i + (j * framesForSymbol) + k]);
				}
				vidWriter << frame;
			}
		}
		// adding one dummy black second to the end of the video
		Utilities::addDummyFramesToVideo(vidWriter, framerate);
		// adding chess board to indicate the end of transmission
		Utilities::addDummyFramesToVideo(vidWriter, framerate, Utilities::createChessBoard());
	}

	// symbol_time: how many milliseconds will the symbol last
	void sendVideo(string inputVideoFile, vector<short> msg, string outputVideoFile, int symbol_time)
	{
		cout << "I am here\n";
		int sections = sectionsPerLength * sectionsPerLength;
		VideoCapture videoReader(inputVideoFile);
		if (videoReader.isOpened())
		{
			videoReader.set(CV_CAP_PROP_POS_FRAMES, 0); //Set index to last frame
			int framerate = videoReader.get(CV_CAP_PROP_FPS); //get the frame rate
			int frame_width = videoReader.get(CV_CAP_PROP_FRAME_WIDTH);
			int frame_height = videoReader.get(CV_CAP_PROP_FRAME_HEIGHT);
			int fps = Utilities::lcm((int)framerate, Utilities::lcm(2 * FREQ[ZERO], 2 * FREQ[ONE]));
			//int frames_per_symbol = (fps * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
			double lumin1[] = { LUMINANCE[0], LUMINANCE[1] };
			vector<float> amplitudes1 = createWaveGivenFPS(fps, msg, symbol_time, FREQ[ZERO], FREQ[ONE], lumin1);
			
			Mat img;
			// create the video writer
			//ostringstream outputVideoStream;
			//outputVideoStream << msg.size() << "_Split" << sections << Utilities::createOuputVideoName(symbol_time, inputVideoFile, outputVideoFile);
			VideoWriter vidWriter = Utilities::getVideoWriter("_Split_" + outputVideoFile, fps, Utilities::getFrameSize());
			int framesForSymbol = (fps * 1000) / symbol_time;

			int inputFrameUsageFrames = fps / framerate;
			// get the sections
			cv::Rect globalROI = Utilities::detectMyBoard(Utilities::createChessBoard());
			vector<cv::Rect> ROIs = Utilities::getDivisions(sections, Utilities::getFrameSize().width, Utilities::getFrameSize().height, 1, false,globalROI);
			// add dummy frames
			videoReader.read(img);
			cv::resize(img, img, Utilities::getFrameSize());
			// add dummy seconds in the beginning of the video
			Utilities::addDummyFramesToVideo(vidWriter, fps / 2, Utilities::createChessBoard());
			Utilities::addDummyFramesToVideo(vidWriter, fps, img.clone() * 0);
			for (int i = 0; i < amplitudes1.size(); i += (sections * framesForSymbol))
			{
				for (int k = 0; k < framesForSymbol; k++)
				{
					if ((i + k)%inputFrameUsageFrames == 0)
					{
						videoReader.read(img);
						cv::resize(img, img, Utilities::getFrameSize());
					}
					Mat frame = img.clone();
					for (int j = 0; j < sections; j++)
					{
						// i is the base, j is the symbol index starting from the base, k is the index of the frameinside the symbol
						Utilities::updateFrameWithAlpha(frame, ROIs[j], amplitudes1[i + (j * framesForSymbol) + k]);
					}
					vidWriter << frame;
				}
			}
			// add dummy frames
			Utilities::addDummyFramesToVideo(vidWriter, fps, img.clone() * 0);
			// end of sending chess
			Utilities::addDummyFramesToVideo(vidWriter, fps / 2, Utilities::createChessBoard());
		}
		cout << endl;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	///              //////////////      Receive     ///////////////                         ////
	/////////////////////////////////////////////////////////////////////////////////////////////
	vector<short> receiveN(vector<vector<float> > frames, int fps, int frames_per_symbol)
	{
		int sections = sectionsPerLength * sectionsPerLength;
		vector<short> result;
		if (frames.size() == 0)
			return result;
		vector<vector<int> > zero_detected(sections,vector<int>(frames[0].size(),0));
		vector<vector<int> > one_detected(sections, vector<int>(frames[0].size(), 0));
		vector<vector<int> > other_detected(sections, vector<int>(frames[0].size(), 0));
		for (int k = 0; k < sections; k++)
		{
			for (int i = 0; i < frames[k].size() - frames_per_symbol; i++)
			{
				vector<Frequency> temp = Utilities::myft(frames[k], fps, i, frames_per_symbol);
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
					for (int j = 0; j < frames_per_symbol; j++) zero_detected[k][i + j]++;
				}
				else if (abs(temp[maxi].freq - FREQ[ONE]) < EPSILON)
				{
					// one detected
					for (int j = 0; j < frames_per_symbol; j++) one_detected[k][i + j]++;
				}
				else
				{
					// other detected
					for (int j = 0; j < frames_per_symbol; j++) other_detected[k][i + j]++;
				}
			}
		}
		// then check for the first frame that has 60% or more with one of the two symbols (0,1), 
		// and the symbol should have enough time (at least after the first FRAMES_PER_SYMBOL have been passed)
		vector<int> starting_indeces(sections,0);
		for (int k = 0; k < sections; k++)
		{
			for (int i = frames_per_symbol; i < frames.size() - frames_per_symbol; i++)
			{
				if ((zero_detected[k][i] + one_detected[k][i]) * 10 >= (zero_detected[k][i] + one_detected[k][i] + other_detected[k][i]) * 6)
				{
					// this first frame and zero
					starting_indeces[k] = i;
					//result.push_back(0);
					break;
				}
				//else if (one_detected[k][i] * 10 >= (zero_detected[k][i] + one_detected[k][i] + other_detected[k][i]) * 6)
				//{
					// this first frame and one
					//starting_indeces[k] = i;
					//result.push_back(1);
					//break;
				//}
			}
		}
		int starting_index = std::max(fps,*(std::min_element(starting_indeces.begin(), starting_indeces.end())));
		// for the rest of the symbols
		// just follow the same rule
		for (int i = starting_index; i < frames[0].size() - frames_per_symbol; i += frames_per_symbol)
		{
			for (int k = 0; k < sections; k++)
			{
				if (zero_detected[k][i] > one_detected[k][i])
				{
					// this first frame and zero
					result.push_back(0);
				}
				else// if (one_detected[k][i] > zero_detected[k][i])
				{
					// this first frame and one
					result.push_back(1);
				}
				/*else
				{
					result.push_back(2);
				}*/
			}
		}
		return result;
	}

	// receive with a certain ROI ratio
	vector<short> receive(string fileName, int frames_per_symbol, double ROI_Ratio)
	{
		int fps = 0;
		vector<vector<float> > frames = Utilities::getVideoFrameLuminancesSplitted(fileName, ROI_Ratio, fps, sectionsPerLength*sectionsPerLength,true);
		return receiveN(frames, 30, frames_per_symbol);
	}
};

