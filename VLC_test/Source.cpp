#include "Properties.h"

#include <random>
#include <iostream>

//int num = 100;
//int range = 1000;
//void main1() {
//	//std::random_device rd;
//	//std::mt19937 mt(rd());
//	std::mt19937 mt(19937);
//	std::uniform_int_distribution<int> dist(1, range);
//	double sum = 0;
//	for (int i = 0; i < num; ++i)
//	{
//		sum += dist(mt);
//	}
//	cout << sum / num << endl;
//}
//void main2() {
//	srand(time(0));
//	double sum = 0;
//	for (int i = 0; i < num; ++i)
//	{
//		sum += ((rand() % range) + 1);
//	}
//	cout << sum / num << endl;
//}

int main(int argc, char** argv)
{
	string release = "C:\\Users\\mostafaizz\\Documents\\Visual Studio 2013\\Projects\\ConsoleApplication1\\Release\\";
	//Utilities::exploreVideo(release + "throughput_fulltree_4freq\\combinedVideo\\IMG_5771.MOV");
	return Properties::getInst()->testSendReceive(argc, argv);
	
	
	return 0;
}