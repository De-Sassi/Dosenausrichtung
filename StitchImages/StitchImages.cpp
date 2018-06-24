// StitchImages.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching.hpp"

using namespace std;
using namespace cv;

bool try_use_gpu = false;
Stitcher::Mode mode = Stitcher::PANORAMA;
vector<Mat> imgs;
string result_name = "result.jpg";
string path = "C:\\Users\\delia\\Desktop\\P4\\Dosenbilder23\\beer_WOB\\flatted\\";

int main()
{
	//read the images
	for (int i = 2; i <= 9; i++)
	{
		Mat img = imread(path + to_string(i) + "flat.jpg");
		if (img.empty())
		{
			cout << "Can't read image '" << (path + to_string(i) + "flat.jpg" )<< "'\n";
			return -1;
		}
		imgs.push_back(img);
	}

	Mat pano;

	Ptr<Stitcher> stitcher = Stitcher::create(mode, try_use_gpu);
	Stitcher::Status status = stitcher->stitch(imgs, pano);

	if (status != Stitcher::OK)
	{
		cout << "Can't stitch images, error code = " << int(status) << endl;
		return -1;
	}

	imwrite(path+result_name, pano);
	 
    return 0;
}

