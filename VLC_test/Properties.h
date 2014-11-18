#pragma once

#include "SplitFrequencyAmplitudeCommunicator.h"
#include "SplitAmplitudeCommunicator.h"
#include "SpatialFrequencyCommunicator.h"
#include "SplitScreenCommunicator.h"
#include "SplitScreenAmpDifferenceCommunicator.h"

enum
{
	SEND = 1,
	RECV,
	CNVRT,
	EXTEND
};

enum
{
	NORMAL = 0,
	AMP_DIFF,
	FREQ_DIFF,
	FREQ_AMP_DIFF
};

struct Properties
{
	int mode; // 0 for send and 1 for receive, 2 for converting video
	string inputFileName; // input file name for processing in case of receive, and input video/image file name in case of send
	string outputFileName; // used in send only
	float ROI; // <= 0 means in the receiver use selection by hand and positive value means percentage
	int type; // -1->the old HiLight work(no difference),0->normal(and default),1->split amplitude,2->split frequency,3->split amplitude and frequency, 4 -> spatial
	bool realVideo; // true means real video and false means not
	string text; // text to send
	vector<short> msg; // the message after conversion to vector<short>
	double fps;
	int extendN;
	Properties()
	{
		mode = SEND;
		realVideo = false;
		outputFileName = "output.avi";
		inputFileName = "";
		ROI = 1;
		type = 0;
		text = "";
		
	}
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
					type = (argv[++i][0] - '0');
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
					string fileName = argv[++i];
					ifstream ifs(fileName);
					if (ifs.is_open())
					{
						// assume the text inside
						ifs >> text;
						ifs.close();
					}
					else					
					{
						// assume the file name is the text
						text = fileName;
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
					fps = stod(string(argv[++i]));
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
			communicator = new SplitScreenCommunicator(2);
			break;
		case 5:
			communicator = new SplitScreenAmpDifferenceCommunicator(1);
			break;
		case 6:
			communicator = new SpatialFrequencyCommunicator;
			break;
		default:
			communicator = new Communicator;
		}
		
		switch (mode)
		{
		case SEND:
			if (realVideo)
			{
				communicator->sendVideo(inputFileName, msg, outputFileName, 1000);
			}
			else
			{
				communicator->sendImage(Utilities::lcm(2 * FREQ[ONE], 2 * FREQ[ZERO]),
					inputFileName, msg, outputFileName, 1000);
			}
			break;
		case RECV:

			if (ROI > 0 && ROI <= 1)
			{
				// then we have ROI
				vector<short> received = communicator->receive(inputFileName, 30, ROI);
				for (int i = 0; i < msg.size(); i++)
				{
					cout << msg[i];
				}
				cout << endl;
				for (int i = 0; i < received.size(); i++)
				{
					cout << received[i];
				}
				cout << endl;
				Utilities::LCS_greedy(msg, received);
			}
			else
			{
				communicator->receiveWithSelectionByHand(inputFileName, 30);
			}
			break;
		case CNVRT:
			// convert argv2 video to argv3 as a video with the framerate in argv4
			// argv3 must end with .avi
			Utilities::convertVideo(inputFileName, outputFileName, fps);
			break;
		case EXTEND:
			// extend the video by repeating
			Utilities::repeatVideo(inputFileName, outputFileName, fps, extendN);
			break;
		}
		
		return 0;
	}
};