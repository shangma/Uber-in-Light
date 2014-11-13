#include "Properties.h"
#include "BCH.h"
#include "Hamming.h"


int main(int argc, char** argv)
{
	//Mat img1 = imread("D:\\MSECE_IUPUI\\MSECE_IUPUI\\Testing_image\\img2.jpg");
	//SpatialFrequency::createOfflineVideoWithGivenFPS(30, "D:\\MSECE_IUPUI\\MSECE_IUPUI\\Testing_image\\img2.jpg", "a", "output.avi", 1000);
	//SpatialFrequency::getVideoFrameLuminances(argv[1]);
	Properties prop;
	//Utilities::compareVideos("..\\Release\\2688345269_104_12Hz_8Hz_1000ms_0.01levels_I420_image_output.avi",
	//	"..\\Release\\2688579461_104_Split1_12Hz_8Hz_1000ms_0.01levels_I420_image_output.avi");
	//return prop.testSendReceive(argc, argv);
	//displayVideo("..\\Debug\\20141009_131021_abcde_30fps_15Hz_10Hz_newblending.mp4");
	//-createHalfAndHalfImage();
	BCH bch;
	bch.testBCH();
	//testHamming();
	//Mat test = VLCCalibration::createChessBoard(0.9, cv::Size(4,4));
	//imshow("test", test);
	//cv::waitKey(0);

	//VLCCalibration::detectMyBoard(test, cv::Size(4,4));
	
	return 0;
}