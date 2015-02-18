#pragma once

#include "Header.h"

enum
{
	FFT_RANDOM_GUESS,
	FFT_NO_RANDOM_GUESS,
	CROSS_CORRELATION,
};

// try to read this from file
struct SymbolData
{
	string symbol;
	double amplitude;
	double frequency;
	double phase; // currently zeros only
	vector<short> getSymbol()
	{
		vector<short> ret;
		for (int i = 0; i < symbol.size(); i++)
		{
			ret.push_back(symbol[i] - '0');
		}
		return ret;
	}
};

struct AllSymbolsData
{
	string fileName;
	double FREQ[2];
	vector<SymbolData> allData;
	// assume the fiel is sorted
	void readData(string fileName)
	{
		ifstream ifstr(fileName);
		if (ifstr.is_open())
		{
			SymbolData obj;
			while (ifstr >> obj.symbol >> obj.frequency >> obj.amplitude >> obj.phase)
			{
				allData.push_back(obj);
			}
		}
		if (allData.size() >= 2)
		{
			this->fileName = fileName;
		}
	}
	string toString()
	{
		if (fileName.size() > 0)
		{
			return fileName;
		}
		ostringstream ostr;
		ostr << FREQ[0] << "Hz_" << FREQ[1];
		return ostr.str();
	}
	void addSymbol(string symbol, double frequency, double amplitude = 0.005, double phase = 0)
	{
		for (int i = 0; i < allData.size(); i++)
		{
			if (symbol == allData[i].symbol)
			{
				cout << "symbol " << symbol << "already exists!" << endl;
				return;
			}
		}
		SymbolData obj;
		obj.symbol = symbol;
		obj.frequency = frequency;
		obj.amplitude = amplitude;
		obj.phase = phase;
		allData.push_back(obj);
		cout << "symbol " << symbol << " added successfully!" << endl;
	}
	SymbolData* getSymbol(string symbol)
	{
		for (int i = 0; i < allData.size(); i++)
		{
			if (symbol == allData[i].symbol)
			{
				return &allData[i];
			}
		}
		return 0;
	}
	vector<SymbolData> getMsgSymbols(vector<short>& msg)
	{
		vector<SymbolData> res;
		for (int i = 0; i < msg.size();i++)
		{
			ostringstream ostr;
			for (int j = 0; j + i < msg.size(); j++,i++)
			{
				ostr << msg[i + j];
				SymbolData* symbolPtr = getSymbol(ostr.str());
				if (symbolPtr != 0)
				{
					res.push_back(*symbolPtr);
					ostr.clear();
					break;
				}
			}
			while (ostr.str().size() > 0)
			{
				// this should be in the last symbol only otherwise there is something wrong
				ostr << 0; // pad zeros
				SymbolData* symbolPtr = getSymbol(ostr.str());
				if (symbolPtr != 0)
				{
					res.push_back(*symbolPtr);
					ostr.clear();
				}
			}
		}
		return res;
	}
};

struct Parameters
{
	static int startingIndex;
	static float symbolTime;
	static cv::Rect globalROI;
	static vector<float> amplitudes;
	static int fps;
	static int DecodingMethod;
	static double LUMINANCE;
	//enum{ ZERO = 0, ONE };
	static string codec; //I420, DIB ,DIVX, XVID
	static cv::Size DefaultFrameSize;
	static Size patternsize;
	static AllSymbolsData symbolsData;
};
int Parameters::startingIndex = 0;
cv::Rect Parameters::globalROI = cv::Rect(0,0,1,1);
vector<float> Parameters::amplitudes = vector<float>();
int Parameters::fps = 0;
float Parameters::symbolTime = 1000;
int Parameters::DecodingMethod = FFT_RANDOM_GUESS;
double Parameters::LUMINANCE = 0.005;
//enum{ ZERO = 0, ONE };
string Parameters::codec = "I420"; //I420, DIB ,DIVX, XVID
cv::Size Parameters::DefaultFrameSize = cv::Size(640, 480);
Size Parameters::patternsize = cv::Size(11, 11);
AllSymbolsData Parameters::symbolsData;