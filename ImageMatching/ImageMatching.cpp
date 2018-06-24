// ImageMatching.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <map>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2\features2d.hpp"
using namespace std;
using namespace cv;

//create feature finding algorithm. KAZE/BRISK
Ptr<BRISK> alg;
bool sortByResponse(const KeyPoint &first, const KeyPoint &second)
{
	return first.response > second.response;
}

vector<uchar> extract_features(string image_path, int vector_size = 32)
{
	Mat image = imread(image_path);
	
	vector<uchar> flatted;
	try 
	{
		
		vector<KeyPoint> keypoints;
		//Finding Image keypoints
		alg->detect(image, keypoints);
		sort(begin(keypoints), end(keypoints), sortByResponse);
		keypoints.resize(32);
		Mat desc;
		alg->compute(image, keypoints, desc);
		//puts all the element in the matrix in one  vector
		if (desc.isContinuous())
		{
			flatted.assign(desc.datastart, desc.dataend);
		}
		else
		{
			for (int i = 0; i < desc.rows; i++)
			{
				flatted.insert(flatted.end(), desc.ptr<uchar>(i), desc.ptr<uchar>(i) + desc.cols);
			}
		}
		
		int needed_size = vector_size * 64;
		flatted.resize(needed_size, 0); //compute right lenght
	

	}
	catch(const Exception& e)
	{
		cout << "Error: " << e.msg<<"\n";
	}
	return flatted;
}

void batch_extractor(string images_path, map<string, vector<uchar>>* result)
{
	//writes image filename together with its desriptors
	for (int i = 2; i <= 9; i++)
	{
		string name = to_string(i) + "cut.jpg";
		(*result)[name]= extract_features(images_path + "\\" + name);
	}
}


// source :https://stackoverflow.com/questions/30404099/right-way-to-compute-cosine-similarity-between-two-arrays
double cosine_similarity(vector<uchar>* A, vector<uchar>* B)
{
	double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
	for (int i = 0; i < A->size(); ++i) {
		dot += A->at(i) * B->at(i);
		denom_a += A->at(i) * A->at(i);
		denom_b += B->at(i) * B->at(i);
	}
	return dot / (sqrt(denom_a) * sqrt(denom_b));
}


bool sortByDistanceAscending(std::pair<std::string, int> elem1, std::pair<std::string, int> elem2)
{
	return elem1.second > elem2.second;
}

string match(map<string, vector<uchar>>* result, string image_path, int topn = 5)
{
	vector<uchar> features = extract_features(image_path);
	map<string, double> matchPerImage;//strng is image name, double is distance
									  //compares the images 
	pair<string, double> isBiggestMatch = std::make_pair("start", 0);
	for (auto element : *result)
	{
		double distance = cosine_similarity(&features, &element.second);
		matchPerImage[element.first] = distance;
		if (distance > isBiggestMatch.second)
		{
			isBiggestMatch = std::make_pair(element.first, distance);
		}
	}
	//Gets the biggest Value-> best match
	string nameImageBestFit = isBiggestMatch.first;
	return nameImageBestFit;

}

int main()
{
	string path = "C:\\Users\\delia\\Desktop\\P4\\Dosenbilder23\\beer_WOB\\cutted\\";
	string imagespath = "C:\\Users\\delia\\Desktop\\P4\\Dosenbilder23\\beer_WOB\\cutted";
	string pathFirst= path+"3cut.jpg";
	
	Mat image=imread(pathFirst);
	//Show
	cv::namedWindow("normal", WINDOW_AUTOSIZE);
	imshow("normal", image);
	alg = BRISK::create();

	map<string, vector<uchar>> descriptors;
	batch_extractor(imagespath, &descriptors);
	string nameImage=match(&descriptors, pathFirst);

	cout << nameImage;
	waitKey();

    return 0;
}




