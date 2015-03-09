#include "Properties.h"
#include "SplitAmplitudeCommunicator.h"
#include "SplitScreenCommunicator.h"
#include "SplitScreenAmpDifferenceCommunicator.h"
#include "OldCommunicator.h"
#include "RGBCommunicator.h"
#include "RGBSpatialRedundancy.h"
#include "HUECommnunicator.h"
#include "BGRCommunicator2.h"
#include "BGRCommunicator3.h"
#include "Hamming.h"
#include "ReedSolomon.h"

//Properties* Properties::inst = Properties::getInst();
int Properties::returnError()
{
	cout << "Usage: run.exe (-s <message>)|(-r <filename>)\n";
	cout << "One option must be selected\n";
	cout << "-s : create video file with the specified message where the message will be converted to binary\n";
	cout << "-r : receive message from video file and write the output to the screen\n";
	return -1;
}
int Properties::testSendReceive(int argc, char** argv)
{

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
			Parameters::realVideo = true;
		}
		else if (!strcmp(argv[i], "-zero"))
		{
			// get the file name
			if (i < argc - 1)
			{
				Parameters::symbolsData.addSymbol("0", stod(string(argv[++i])));
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
				Parameters::symbolsData.addSymbol("1", stod(string(argv[++i])));
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
				string start = string(argv[++i]);
				ifstream startStream(start);
				if (startStream.is_open())
				{
					startStream >> Parameters::start_second;
					Parameters::endSecondFile = start;
				}
				else
				{
					Parameters::start_second = stod(start);
				}
				startStream.close();
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
				Parameters::end_second = stod(string(argv[++i]));
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
				Parameters::sideA = Parameters::sideB = stoi(string(argv[++i]));
			}
			else
			{
				return returnError();
			}
		}
		else if (!strcmp(argv[i], "-sideA"))
		{
			// the starting second of the video
			// currntly in the conversion only
			if (i < argc - 1)
			{
				Parameters::sideA = stoi(string(argv[++i]));
			}
			else
			{
				return returnError();
			}
		}
		else if (!strcmp(argv[i], "-sideB"))
		{
			// the starting second of the video
			// currntly in the conversion only
			if (i < argc - 1)
			{
				Parameters::sideB = stoi(string(argv[++i]));
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
		else if (!strcmp(argv[i], "-decode"))
		{
			// the decoding method
			if (i < argc - 1)
			{
				Parameters::DecodingMethod = stoi(string(argv[++i]));
			}
			else
			{
				return returnError();
			}
		}
		else if (!strcmp(argv[i], "-amp"))
		{
			// the amplitude
			if (i < argc - 1)
			{
				//Parameters::LUMINANCE = stod(string(argv[++i]));
			}
			else
			{
				return returnError();
			}
		}
		else if (!strcmp(argv[i], "-time"))
		{
			// the amplitude
			if (i < argc - 1)
			{
				Parameters::symbolTime = stod(string(argv[++i]));
			}
			else
			{
				return returnError();
			}
		}
		else if (!strcmp(argv[i], "-symbols"))
		{
			if (i < argc - 1)
			{
				Parameters::symbolsData.readData(string(argv[++i]));
			}
			else
			{
				return returnError();
			}
		}
		else if (!strcmp(argv[i], "-res"))
		{
			if (i < argc - 2)
			{
				Parameters::DefaultFrameSize.width = stoi(argv[++i]);
				Parameters::DefaultFrameSize.height = stoi(argv[++i]);
			}
			else
			{
				return returnError();
			}
		}
		else if (!strcmp(argv[i], "-full"))
		{
			if (i < argc - 1)
			{
				Parameters::fullScreen = stoi(string(argv[++i]));
			}
			else
			{
				return returnError();
			}
		}
		else if (!strcmp(argv[i], "-codec"))
		{
			if (i < argc - 1)
			{
				Parameters::setCodec(string(argv[++i]));
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
	case SPATIAL_REDUNDANCY:
		communicator = new SplitAmplitudeCommunicator;
		break;
	case FREQ_DIFF:
		//communicator = new SplitFrequencyCommunicator;
		break;
	case FREQ_AMP_DIFF:
		//communicator = new SplitFrequencyAmplitudeCommunicator;
		break;
	case SPLIT_SCREEN_TEMPORAL_CORRELATION:
		communicator = new SplitScreenCommunicator;
		break;
	case SPLIT_SCREEN_SPATIAL_REDUNDANCY:
		communicator = new SplitScreenAmpDifferenceCommunicator;
		break;
	case 6:
		communicator = new RGBCommunicator;
		break;
	case 7:
		communicator = new RGBSpatialRedundancyCommunicator;
		break;
	case 8:
		communicator = new HUECommunicator;
		break;
	case 9:
		communicator = new BGRCommunicator2;
		break;
	case 10:
		communicator = new BGRCommunicator3;
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
			msg = hamming.EncodeMessage(msg, false);
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
		if (Parameters::realVideo)
		{
			if (communicator->initVideo(inputFileName, msg,
				Utilities::createOuputVideoName(msgFileName, inputFileName, outputFileName)))
			{
				communicator->sendVideo();
			}
		}
		else
		{
			if (communicator->initImage(inputFileName, msg,
				Utilities::createOuputVideoName(msgFileName, inputFileName, outputFileName)))
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
				received = communicator->receive(inputFileName, ROI);
			}
			else
			{
				received = communicator->receiveColor(inputFileName, ROI, cv::Scalar(0, 0, 230));
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
			communicator->receiveWithSelectionByHand(inputFileName);
		}
		break;
	case CNVRT:
		// convert argv2 video to argv3 as a video with the framerate in argv4
		// argv3 must end with .avi
		Utilities::convertVideo(inputFileName, outputFileName, Parameters::fps, Parameters::start_second, Parameters::end_second);
		break;
	case EXTEND:
		// extend the video by repeating
		Utilities::repeatVideo(inputFileName, outputFileName, Parameters::fps, extendN, Parameters::start_second, Parameters::end_second);
		break;
	case CORRELEATION:
		Utilities::correlateVideoDifference(inputFileName, Parameters::start_second, Parameters::end_second, correlation);
		break;
	}

	return 0;
}
vector<Mat> Properties::getSplittedImages(Mat &frame)
{
	return communicator->getSplittedImages(frame);
}

int main(int argc, char** argv)
{
	string release = "C:\\Users\\mostafaizz\\Documents\\Visual Studio 2013\\Projects\\ConsoleApplication1\\Release\\";
	//Mat img = imread(release + "img2.jpg");
	//Utilities::calcHistogram(img);
	//Utilities::exploreVideo(release + "HiLight\\IMG_6049.MOV");
	return Properties::getInst()->testSendReceive(argc, argv);
	
	
	return 0;
}