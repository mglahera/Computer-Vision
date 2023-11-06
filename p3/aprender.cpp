#include "opencv2/opencv.hpp"
#include "iostream"
#include "functions.h"
#include <string>
#include <map>

using namespace std;

void train_all(string, map<string,int>);
void train_obj(string, string, map<string,int>);


int main(int argc, char* argv[])
{
	map<string, int> objects;
	objects["circulo"] = 0;
	objects["rectangulo"] = 1;
	objects["rueda"] = 2;
	objects["triangulo"] = 3;
	objects["vagon"] = 4;

	if (argc == 2 )
	{
		string path = argv[1];
		train_all(path, objects);
	}
	else if (argc == 3)
	{
		string path = argv[1];
		string obj = argv[2];
		train_obj(path, obj, objects);
	}
	else {
		cout << "Usage: ./aprender <nomfich> <nombobj>" << endl;
		cout << "Or: ./aprender <objectsDirectory>" << endl;
		return -1;
	}	
}

void train_all(string path, map<string,int> objects) 
{
	vector<cv::String> files;
	glob(path, files, false);

	for (int i = 0; i < files.size(); i++)
	{
		string file = files[i];
		cout << "Reading: " << file << endl;
		string obj = file.substr(0, file.size() - 5);
		obj = obj.substr(11, obj.size());

		cv::Mat img = imread(file, cv::IMREAD_GRAYSCALE);
		if (img.empty())
		{
			cout << "ERROR reading image: " << file << endl;
		}
		imshow("Output with Otsu", img);
		cv::waitKey(0);

		cv::Mat res = threshold(img);

		vector<vector<cv::Point>> contours = getBlobs(res);

		getDescriptors(contours, objects[obj]);
	}
}

void train_obj(string path, string obj, map<string, int> objects)
{
	cv::Mat img = imread(path, cv::IMREAD_GRAYSCALE);
	if (img.empty())
	{
		cout << "ERROR reading image: " << path << endl;
	}

	cv::Mat res = threshold(img);

	vector<vector<cv::Point>> contours = getBlobs(res);

	getDescriptors(contours, objects[obj]);
}
