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

#include "Properties.h"
#include "SplitAmplitudeCommunicator.h"
#include "SplitScreenCommunicator.h"
#include "SplitScreenAmpDifferenceCommunicator.h"
#include "OldCommunicator.h"
#include "BGR0Communicator.h"
#include "RGBSpatialRedundancy.h"
#include "HUECommnunicator.h"
#include "BGRCommunicator2.h"
#include "BGRCommunicator3.h"
#include "BGRCommunicator4.h"
#include "B0GRCommunicator.h"
#include "BGR0Communicator.h"
#include "Hamming.h"
#include "ReedSolomon.h"
#include "mygl.h"

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
		else if (!strcmp(argv[i], "-exp"))
		{
			puts("explore ");
			mode = EXPLORE;
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
				Parameters::outputFileName = argv[++i];
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
		else if (!strcmp(argv[i], "-live"))
		{
			Parameters::liveTranmitter = 1;
		}
		else if (!strcmp(argv[i], "-total"))
		{
			// the amplitude
			if (i < argc - 1)
			{
				Parameters::totalTime = stoi(string(argv[++i]));
			}
			else
			{
				return returnError();
			}
		}
		else if (!strcmp(argv[i], "-seed"))
		{
			// the amplitude
			if (i < argc - 1)
			{
				Parameters::seed = stoi(string(argv[++i]));
			}
			else
			{
				return returnError();
			}
		}
	}
	if (Parameters::totalTime)
	{
		int totalLength = Parameters::getSymbolLength() *  Parameters::sideA * Parameters::sideB * 1000 * Parameters::totalTime / Parameters::symbolTime;
		//printf("length=%d\n", totalLength);
		msg.clear();
		std::mt19937 mt(19937);
		std::uniform_int_distribution<int> dist(0, 1);
		for (int i = 0; i < totalLength; i++)
		{
			msg.push_back(dist(mt));
			//printf("%d", msg[i]);
		}
		ostringstream ostr;
		ostr << "totaltime" << Parameters::totalTime << "_seed" << Parameters::seed;
		msgFileName = ostr.str();
		//puts("");
	}
	else if (msgFileName.size())
	{
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
	else if (mode == SEND || mode == RECV)
	{
		return returnError();
	}

	if (inputFileName == "")
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
		communicator = new BGRCommunicator;
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
	case 11:
		communicator = new BGRCommunicator4;
		break; 
	case 12:
		communicator = new B0GRCommunicator;
		break;
	case 13:
		communicator = new BGR0Communicator;
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
	{
		std::thread startTrans(displayGlut, argc, argv);
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
				Utilities::createOuputVideoName(msgFileName, inputFileName, Parameters::outputFileName)))
			{
				communicator->sendVideo();
			}
		}
		else
		{
			if (communicator->initImage(inputFileName, msg,
				Utilities::createOuputVideoName(msgFileName, inputFileName, Parameters::outputFileName)))
			{
				communicator->sendImage();
			}
		}
		startTrans.join();
		break;
	}
	case RECV:

		if (ROI > 0 && ROI <= 1)
		{

			// then we have ROI
			vector<short> received;
			//if (!color)
			//{
				received = communicator->receive(inputFileName, ROI);
			/*}
			else
			{
				received = communicator->receiveColor(inputFileName, ROI, cv::Scalar(0, 0, 230));
			}*/
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
			// calculate symbol errors
			vector<SymbolData> msgSymbols = Parameters::symbolsData.getMsgSymbols(msg);
			vector<SymbolData> recSymbols = Parameters::symbolsData.getMsgSymbols(received);
			//Utilities::LCS_greedy(msgSymbols, recSymbols);
		}
		/*else
		{
			communicator->receiveWithSelectionByHand(inputFileName);
		}*/
		break;
	case CNVRT:
		// convert argv2 video to argv3 as a video with the framerate in argv4
		// argv3 must end with .avi
		Utilities::convertVideo(inputFileName, Parameters::outputFileName, Parameters::fps, Parameters::start_second, Parameters::end_second);
		break;
	case EXTEND:
		// extend the video by repeating
		Utilities::repeatVideo(inputFileName, Parameters::outputFileName, Parameters::fps, extendN, Parameters::start_second, Parameters::end_second);
		break;
	case CORRELEATION:
		Utilities::correlateVideoDifference(inputFileName, Parameters::start_second, Parameters::end_second, correlation);
		break;
	case EXPLORE:
		Utilities::exploreVideo(inputFileName);
		break;

	}

	return 0;
}
vector<Mat> Properties::getSplittedImages(Mat &frame)
{
	return communicator->getSplittedImages(frame);
}

#include <omp.h>

int main(int argc, char** argv)
{
	//omp_set_dynamic(0);
	int num = omp_get_num_procs();
	omp_set_num_threads(num / 2); // half of the processors
	/*Mat res = Utilities::getGaussian2D(1280, 720);

	cout << res.cols << endl;
	cout << res.rows << endl;
	double minV, maxV;
	minMaxIdx(res, &minV, &maxV);
	res -= minV;
	res /= (maxV - minV);
	imshow("kernel", res);
	cv::waitKey();
	return 0;*/
	/*vector<float> v = Utilities::getSynchSignal(30);
	for (int i = 0; i < v.size(); i++)
		cout << v[i] << endl;
	return 0;*/
	string release = "C:\\VLC\\Release\\";
	//string fileName = release + string(argv[1]) + ".rand";
	//string dstfileName = release + string(argv[1]) + "org.rand";
	//ifstream ifs(fileName);
	//// assume the text inside
	//string text;
	//ifs >> text;
	//ifs.close();
	//// convert the message to vector of short
	//vector<short> msg = Utilities::getBinaryMessage(text);

	//AllSymbolsData asd1,asd2;
	//asd1.readData(release + "8freq8.symbol");
	//asd2.readData(release + "8freq8org.symbol");

	//vector<SymbolData> msg1 = asd1.getMsgSymbols(msg);
	//ofstream dst(dstfileName);
	//for (int i = 0; i < msg1.size(); i++)
	//{
	//	double freq = msg1[i].frequency;
	//	for (int j = 0; j < asd2.allData.size(); j++)
	//	{
	//		if (abs(freq - asd2.allData[j].frequency) < 0.1)
	//		{
	//			dst << asd2.allData[j].symbol;
	//		}
	//	}
	//}
	//dst.close();

	//Mat img = imread(release + "img2.jpg");
	//Utilities::calcHistogram(img);
	// 33 -> 38
	// Utilities::exploreVideo(release + "RGB2_GREEN_SYNCH\\20150318_161625_775741095__RGB2_2x3rand_8Freq8symbol_sideA3_sideB2_full1_300ms_levels_XVID_Tree24_whole_videoavi_output.mp4");
	// 26 -> 35
	// Utilities::exploreVideo(release + "RGB2_GREEN_SYNCH\\20150318_161711_775767883__RGB2_5x6rand_8Freq8symbol_sideA6_sideB5_full1_300ms_levels_XVID_Tree24_whole_videoavi_output.mp4");
	// 26 -> 17
	// Utilities::exploreVideo(release + "RGB2_GREEN_SYNCH\\20150318_161755_775793936__RGB2_10x12rand_8Freq8symbol_sideA12_sideB10_full1_300ms_levels_XVID_Tree24_whole_videoavi_output.mp4");
	// 31 -> 31
	// Utilities::exploreVideo(release + "RGB2_GREEN_SYNCH\\20150318_161838_775820157__RGB2_12x15rand_8Freq8symbol_sideA15_sideB12_full1_300ms_levels_XVID_Tree24_whole_videoavi_output.mp4");
	// 27 -> 33
	// Utilities::exploreVideo(release + "RGB2_GREEN_SYNCH\\20150318_161921_775846540__RGB2_15x16rand_8Freq8symbol_sideA16_sideB15_full1_300ms_levels_XVID_Tree24_whole_videoavi_output.mp4");
	// 26 -> 35
	// Utilities::exploreVideo(release + "RGB2_GREEN_SYNCH\\20150318_162002_775872838__RGB2_18x20rand_8Freq8symbol_sideA20_sideB18_full1_300ms_levels_XVID_Tree24_whole_videoavi_output.mp4");
	// 36 -> 36
	//Utilities::exploreVideo(release + "8Freq4org300msRGB2ThroughputLive\\1390782957__RGB2_totaltime60_seed1_8Freq4orgsymbol_sideA3_sideB2_full1_300ms_levels_XVID_d1mp4_output.avi");
	//Utilities::exploreVideo(release + "HiLightLowFreq\\combined\\20150503_155751.mp4");
	return Properties::getInst()->testSendReceive(argc, argv);
	
	
	return 0;
}