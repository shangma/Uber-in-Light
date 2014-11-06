#include "SplitFrequencyAmplitudeCommunicator.h"
#include "SplitAmplitudeCommunicator.h"
#include "SpatialFrequencyCommunicator.h"

enum
{
	SEND = 1,
	RECV,
	CNVRT
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
	double fps;
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
		for (int i = 1; i < argc;i++)
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
					type = (argv[++i][0]-'0');
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
					text = argv[++i];
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
			communicator = new SpatialFrequencyCommunicator;
			break;
		default:
			communicator = new Communicator;
		}
		if (mode == SEND)
		{
			if (realVideo)
			{
				communicator->sendVideo(inputFileName, text, outputFileName, 1000);
			}
			else
			{
				communicator->sendImage(Utilities::lcm(2 * FREQ[ONE], 2 * FREQ[ZERO]),
					inputFileName, text, outputFileName, 1000);
			}
		}
		else if (mode == RECV)
		{
			if (ROI > 0 && ROI <= 1)
			{
				// then we have ROI
				communicator->receive(inputFileName, 30, ROI);
			}
			else
			{
				communicator->receiveWithSelectionByHand(inputFileName, 30);
			}
		}
		else if (mode == CNVRT)
		{
			// convert argv2 video to argv3 as a video with the framerate in argv4
			// argv3 must end with .avi
			Utilities::convertVideo(inputFileName, outputFileName, fps);
		}
		return 0;
	}
};

int main(int argc, char** argv)
{
	//SpatialFrequency::createOfflineVideoWithGivenFPS(30, "D:\\MSECE_IUPUI\\MSECE_IUPUI\\Testing_image\\img2.jpg", "a", "output.avi", 1000);
	//SpatialFrequency::getVideoFrameLuminances(argv[1]);
	Properties prop;
	return prop.testSendReceive(argc, argv);
	//displayVideo("..\\Debug\\20141009_131021_abcde_30fps_15Hz_10Hz_newblending.mp4");
	//-createHalfAndHalfImage();
	return 0;
}