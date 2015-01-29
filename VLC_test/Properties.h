#pragma once

#include "SplitFrequencyAmplitudeCommunicator.h"
#include "SplitAmplitudeCommunicator.h"
#include "SpatialFrequencyCommunicator.h"
#include "SplitScreenCommunicator.h"
#include "SplitScreenAmpDifferenceCommunicator.h"
#include "OldCommunicator.h"
#include "Hamming.h"
#include "ReedSolomon.h"

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
	NORMAL = 0,
	AMP_DIFF,
	FREQ_DIFF,
	FREQ_AMP_DIFF
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
	static Properties* inst;
	Properties()
	{
		mode = SEND;
		realVideo = false;
		outputFileName = "output.avi";
		inputFileName = "";
		ROI = 1;
		type = 0;
		side = 1;
		text = "";
		errorCorrection = 0;
		interleave = false;
		starting_second = 0;
		ending_second = 0;
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
	int side; // number of cells per side
	bool realVideo; // true means real video and false means not
	string text; // text to send
	vector<short> msg; // the message after conversion to vector<short>
	int extendN;
	int errorCorrection;
	bool interleave;
	bool color;
	double starting_second, ending_second; // starting and ending times for processing, currently in the conversion only
	int correlation;
	
	int returnError()
	{
		cout << "Usage: run.exe (-s <message>)|(-r <filename>)\n";
		cout << "One option must be selected\n";
		cout << "-s : create video file with the specified message where the message will be converted to binary\n";
		cout << "-r : receive message from video file and write the output to the screen\n";
		return -1;
	}
	int testSendReceive(int argc, char** argv)
	{
		Communicator *communicator;
		for (int i = 1; i < argc; i++)
		{
			if (!strcmp(argv[i], "-s"))
			{
				mode = SEND;
			}
			else if (!strcmp(argv[i], "-r"))
			{
				mode = RECV;
			}
			else if (!strcmp(argv[i], "-c"))
			{
				mode = CNVRT;
			}
			else if (!strcmp(argv[i], "-e"))
			{
				mode = EXTEND;
				// get number of repitionis
				if (i < argc - 1)
				{
					extendN = stod(string(argv[++i]));
				}
				else
				{
					return returnError();
				}
			}
			else if (!strcmp(argv[i], "-if"))
			{
				// get the file name
				if (i < argc - 1)
				{
					inputFileName = argv[++i];
				}
				else
				{
					return returnError();
				}
			}
			else if (!strcmp(argv[i], "-of"))
			{
				// get the file name
				if (i < argc - 1)
				{
					outputFileName = argv[++i];
				}
				else
				{
					return returnError();
				}
			}
			else if (!strcmp(argv[i], "-m"))
			{
				// get the file name
				if (i < argc - 1)
				{
					type = stoi(argv[++i]);
				}
				else
				{
					return returnError();
				}
			}
			else if (!strcmp(argv[i], "-t"))
			{
				// get the file name
				if (i < argc - 1)
				{
					msgFileName = argv[++i];
					ifstream ifs(msgFileName);
					if (ifs.is_open())
					{
						// assume the text inside
						ifs >> text;
						ifs.close();
					}
					else					
					{
						// assume the file name is the text
						text = msgFileName;
					}
					// convert the message to vector of short
					msg = Utilities::getBinaryMessage(text);
				}
				else
				{
					return returnError();
				}
			}
			else if (!strcmp(argv[i], "-roi"))
			{
				// get the file name
				if (i < argc - 1)
				{
					ROI = stod(string(argv[++i]));
				}
				else
				{
					return returnError();
				}
			}
			else if (!strcmp(argv[i], "-fps"))
			{
				// get the file name
				if (i < argc - 1)
				{
					Parameters::fps = stod(string(argv[++i]));
				}
				else
				{
					return returnError();
				}
			}
			else if (!strcmp(argv[i], "-v"))
			{
				realVideo = true;
			}
			else if (!strcmp(argv[i], "-zero"))
			{
				// get the file name
				if (i < argc - 1)
				{
					FREQ[ZERO] = stod(string(argv[++i]));
				}
				else
				{
					return returnError();
				}
			}
			else if (!strcmp(argv[i], "-one"))
			{
				// get the file name
				if (i < argc - 1)
				{
					FREQ[ONE] = stod(string(argv[++i]));
				}
				else
				{
					return returnError();
				}
			}
			else if (!strcmp(argv[i], "-ec"))
			{
				// error correction enabled
				// the starting second of the video
				// currntly in the conversion only
				if (i < argc - 1)
				{
					errorCorrection = stoi(string(argv[++i]));
				}
				else
				{
					return returnError();
				}
			}
			else if (!strcmp(argv[i], "-interleave"))
			{
				// error correction enabled and interleave enabled
				// doesn't mean anything if error correction is not enabled
				interleave = true;
			}
			else if (!strcmp(argv[i], "-start"))
			{
				// the starting second of the video
				// currntly in the conversion only
				if (i < argc - 1)
				{
					starting_second = stod(string(argv[++i]));
				}
				else
				{
					return returnError();
				}
			}
			else if (!strcmp(argv[i], "-end"))
			{
				// the starting second of the video
				// currntly in the conversion only
				if (i < argc - 1)
				{
					ending_second = stod(string(argv[++i]));
				}
				else
				{
					return returnError();
				}
			}
			else if (!strcmp(argv[i], "-side"))
			{
				// the starting second of the video
				// currntly in the conversion only
				if (i < argc - 1)
				{
					side = stoi(string(argv[++i]));
				}
				else
				{
					return returnError();
				}
			}
			else if (!strcmp(argv[i], "-color"))
			{
				// the starting second of the video
				// currntly in the conversion only
				color = true;
			}
			else if (!strcmp(argv[i], "-corr"))
			{
				// the starting second of the video
				// currntly in the conversion only
				if (i < argc - 1)
				{
					mode = CORRELEATION;
					correlation = stoi(string(argv[++i]));
				}
				else
				{
					return returnError();
				}
			}
		}
		if (inputFileName == "")
		{
			return returnError();
		}
		if (mode == SEND && text == "")
		{
			return returnError();
		}
		switch (type)
		{
		case 1:
			communicator = new SplitAmplitudeCommunicator;
			break;
		case 2:
			communicator = new SplitFrequencyCommunicator;
			break;
		case 3:
			communicator = new SplitFrequencyAmplitudeCommunicator;
			break;
		case 4:
			communicator = new SplitScreenCommunicator(side);
			break;
		case 5:
			communicator = new SplitScreenAmpDifferenceCommunicator(side);
			break;
		case 6:
			communicator = new SpatialFrequencyCommunicator;
			break;
		case -1:
			communicator = new OldCommunicator;
			break;
		default:
			communicator = new Communicator;
		}
		
		switch (mode)
		{
		case SEND:
			if (errorCorrection == HAMMING){
				MyHamming hamming;
				msg = hamming.EncodeMessage(msg,false);
				msgFileName = "Hamming_" + msgFileName;
				if (interleave)
				{
					msgFileName = "Interleave_" + msgFileName;
				}
			}
			else if (errorCorrection == REED_SOLOMON)
			{
				ReedSolomon reedSolomon;
				msg = reedSolomon.encode_bit_stream(msg);
				msgFileName = "ReedSolomon_" + msgFileName;
			}
			if (realVideo)
			{
				if (communicator->initVideo(inputFileName, msg,
					Utilities::createOuputVideoName(msgFileName, 1000, inputFileName, outputFileName), 1000))
				{
					communicator->sendVideo();
				}
			}
			else
			{
				if (communicator->initImage(Utilities::getOuputVideoFrameRate(1),
					inputFileName, msg,
					Utilities::createOuputVideoName(msgFileName, 1000, inputFileName, outputFileName), 1000))
				{
					communicator->sendImage();
				}
			}
			break;
		case RECV:

			if (ROI > 0 && ROI <= 1)
			{
				
				// then we have ROI
				vector<short> received;
				if (!color)
				{
					received = communicator->receive(inputFileName, Parameters::framesPerSymbol, ROI);
				}
				else
				{
					received = communicator->receiveColor(inputFileName, Parameters::framesPerSymbol, ROI,cv::Scalar(0,0,230));
				}
				for (int i = 0; i < msg.size(); i++)
				{
					cout << msg[i];
				}
				if (errorCorrection == HAMMING){
					MyHamming hamming;
					received = hamming.DecodeMessage(received, false);
				}
				else if (errorCorrection == REED_SOLOMON)
				{
					ReedSolomon reedSolomon;
					received = reedSolomon.decode_bit_stream(received);
				}
				
				cout << endl;
				for (int i = 0; i < received.size(); i++)
				{
					cout << received[i];
				}
				cout << endl;
				Utilities::LCS_greedy(msg, received);// , inputFileName);
			}
			else
			{
				communicator->receiveWithSelectionByHand(inputFileName, Parameters::framesPerSymbol);
			}
			break;
		case CNVRT:
			// convert argv2 video to argv3 as a video with the framerate in argv4
			// argv3 must end with .avi
			Utilities::convertVideo(inputFileName, outputFileName, Parameters::fps, starting_second, ending_second);
			break;
		case EXTEND:
			// extend the video by repeating
			Utilities::repeatVideo(inputFileName, outputFileName, Parameters::fps, extendN, starting_second, ending_second);
			break;
		case CORRELEATION:
			Utilities::correlateVideoDifference(inputFileName, starting_second, ending_second, correlation);
			break;
		}
		
		return 0;
	}
};

Properties* Properties::inst = Properties::getInst();
