#pragma once
#include "Parameters.h"

extern class Communicator;

enum
{
	SEND = 1,
	RECV,
	CNVRT,
	EXTEND,
	CORRELEATION
};

enum
{
	TEMPORAL_CORRELATION = 0,
	SPATIAL_REDUNDANCY,
	FREQ_DIFF,
	FREQ_AMP_DIFF,
	SPLIT_SCREEN_TEMPORAL_CORRELATION,
	SPLIT_SCREEN_SPATIAL_REDUNDANCY
};

enum
{
	NO_ERROR_CORRECTION = 0,
	HAMMING,
	REED_SOLOMON
};

class Properties
{
private:
	Communicator*communicator = 0;
	static Properties* inst;
	Properties()
	{
		mode = SEND;
		outputFileName = "output.avi";
		inputFileName = "";
		ROI = 1;
		type = 0;
		text = "";
		errorCorrection = 0;
		interleave = false;
		color = false;
		correlation = 0;
	}
public:
	static Properties* getInst()
	{
		if (inst == 0)
		{
			inst = new Properties();
		}
		return inst;
	}

	int mode; // 0 for send and 1 for receive, 2 for converting video
	string inputFileName; // input file name for processing in case of receive, and input video/image file name in case of send
	string outputFileName; // used in send only
	string msgFileName; // the message file Name
	float ROI; // <= 0 means in the receiver use selection by hand and positive value means percentage
	int type; // -1->the old HiLight work(no difference),0->normal(and default),1->split amplitude,2->split frequency,3->split amplitude and frequency, 4 -> split screen, 5-> split screen and amplitude
	string text; // text to send
	vector<short> msg; // the message after conversion to vector<short>
	int extendN;
	int errorCorrection;
	bool interleave;
	bool color;
	int correlation;
	
	int returnError();
	int testSendReceive(int argc, char** argv);
	vector<cv::Mat> getSplittedImages(cv::Mat &frame);
};

Properties* Properties::inst = Properties::getInst();
