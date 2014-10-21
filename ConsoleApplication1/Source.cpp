#include "SendReceive.h"
#include "SplitFrameSendReceive.h"

int testSendReceive(int argc, char** argv)
{
	//typedef std::chrono::high_resolution_clock Clock;
	//typedef std::chrono::microseconds microseconds;
	//Clock::time_point t0 = Clock::now();
	//try
	//{
	//	//DisplatStabilizeVideo("..\\Debug\\20140925_131350_abcdefghijklm_90_degrees_handheld.mp4", "..\\Debug\\stabilized.avi");
	//	stabilizeVideo2("..\\Debug\\20140925_150121_1bcdefghijklm_handheld.mp4");
	//	//stabilizeVideo("..\\Debug\\20140925_150121_1bcdefghijklm_handheld.mp4");
	//}
	//catch (...)
	//{
	//	puts("something");
	//}
	//microseconds ms = std::chrono::duration_cast<microseconds>(Clock::now() - t0);
	//cout << ms.count() << " microseconds" << endl;
	////
	//return 0;
	//extractAllVideoFrames("D:\\VLC_1st_Demo\\MVI.MOV");
	if (argc < 3)
	{
		cout << "Usage: run.exe (-s <message>)|(-r <filename>)\n";
		cout << "One option must be selected\n";
		cout << "-s : create video file with the specified message where the message will be converted to binary\n";
		cout << "-r : receive message from video file and write the output to the screen\n";
		return 0;
	}
	if (!strcmp(argv[1], "-s"))
	{
		if (argc == 4)
		{
			SplitFrameSendReceive::createOfflineVideoFromVideo(argv[3], argv[2], "Output_outputVideoFileVideo.avi", 1000);
		}
		else
		{
			SendReceive::createOfflineVideoWithGivenFPS(2 * FREQ[ONE] * (FREQ[ZERO] / gcd((int)FREQ[ZERO], (int)FREQ[ONE])),
				"D:\\MSECE_IUPUI\\MSECE_IUPUI\\Testing_image\\img2.jpg", argv[2], "output.avi", 1000);
			//createOfflineVideo("D:\\MSECE_IUPUI\\MSECE_IUPUI\\Testing_image\\img2.jpg", argv[2], "output_old.avi", 1000);
		}
	}
	else if (!strcmp(argv[1], "-r"))
	{
		//receive("D:\\testing videos\\20140918_115116.mp4");
		if (argc == 4)
		{
			// then we have ROI
			cout << "here = " << argv[3] << endl;
			SendReceive::receiveWithInputROIRatio(argv[2], 30, stod(string(argv[3])));
		}
		else
		{
			SendReceive::receiveWithInputROIRectangle(argv[2], 30);
		}
	}
	return 0;
}

void createHalfAndHalfImage()
{
	Mat img = imread("D:\\MSECE_IUPUI\\MSECE_IUPUI\\Testing_image\\img2.jpg");
	Utilities::updateFrameWithAlpha(img, cv::Rect(0, 0, img.cols / 2, img.rows), 1);
	Utilities::updateFrameWithAlpha(img, cv::Rect(img.cols / 2, 0, img.cols / 2, img.rows), -7);

	imshow("img", img);
	cv::waitKey(0);

	Mat HSV;
	cv::cvtColor(img, HSV, CV_BGR2HSV);

	vector<Mat> hsv;
	cv::split(HSV, hsv);

	cv::Scalar first = cv::mean(hsv[2](cv::Rect(0, 0, img.cols / 2, img.rows)));
	cv::Scalar second = cv::mean(hsv[2](cv::Rect(img.cols/2, 0, img.cols / 2, img.rows)));

	cout << first.val[0] << endl;
	cout << second.val[0] << endl;

	imshow("value", hsv[2]);
	cv::waitKey(0);
}

int main(int argc, char** argv)
{
	return testSendReceive(argc,argv);
	//displayVideo("..\\Debug\\20141009_131021_abcde_30fps_15Hz_10Hz_newblending.mp4");
	//-createHalfAndHalfImage();
	return 0;
}