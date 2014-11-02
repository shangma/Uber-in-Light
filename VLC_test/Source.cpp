#include "SplitFrequencyAmplitudeCommunicator.h"
#include "SplitAmplitudeCommunicator.h"
#include "SpatialFrequencyCommunicator.h"

int testSendReceive(int argc, char** argv)
{
	if (argc < 3)
	{
		cout << "Usage: run.exe (-s <message>)|(-r <filename>)\n";
		cout << "One option must be selected\n";
		cout << "-s : create video file with the specified message where the message will be converted to binary\n";
		cout << "-r : receive message from video file and write the output to the screen\n";
		return 0;
	}
	Communicator *communicator = new SpatialFrequencyCommunicator;
	if (!strcmp(argv[1], "-s"))
	{
		if (argc == 4)
		{
			communicator->sendVideo(argv[3], argv[2], "outputVideoFileVideo.avi", 1000);
		}
		else
		{
			communicator->sendImage(Utilities::lcm(2 * FREQ[ONE], 2 * FREQ[ZERO]),
				"D:\\MSECE_IUPUI\\MSECE_IUPUI\\Testing_image\\img2.jpg", argv[2], "output.avi", 1000);
		} 
	}
	else if (!strcmp(argv[1], "-r"))
	{
		if (argc == 4)
		{
			// then we have ROI
			cout << "here = " << argv[3] << endl;
			communicator->receive(argv[2], 30, stod(string(argv[3])));
		}
		else
		{
			communicator->receiveWithSelectionByHand(argv[2], 30);
		}
	}
	else if (!strcmp(argv[1], "-c"))
	{
		// convert argv2 video to argv3 as a video with the framerate in argv4
		// argv3 must end with .avi
		Utilities::convertVideo(argv[2], argv[3], stod(string(argv[4])));
	}
	return 0;
}

int main(int argc, char** argv)
{
	//SpatialFrequency::createOfflineVideoWithGivenFPS(30, "D:\\MSECE_IUPUI\\MSECE_IUPUI\\Testing_image\\img2.jpg", "a", "output.avi", 1000);
	//SpatialFrequency::getVideoFrameLuminances(argv[1]);
	return testSendReceive(argc,argv);
	//displayVideo("..\\Debug\\20141009_131021_abcde_30fps_15Hz_10Hz_newblending.mp4");
	//-createHalfAndHalfImage();
	return 0;
}