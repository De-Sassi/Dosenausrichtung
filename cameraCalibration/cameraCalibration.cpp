// cameraCalibration.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define _USE_MATH_DEFINES //cmath is included somwhere else in the header so it must be first to use the define
#include <cmath>

//opencv
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

//brisk
#include  <opencv2/features2d.hpp>
//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>


using namespace cv;
using namespace std;


string path = "C:\\Users\\delia\\Desktop\\P4\\bver\\";
 
//
//bool loadsCameraCalibration(string name, Mat& cameraMatrix, Mat& distanceCoefficients)
//{
//	cameraMatrix = Mat(Size(3, 3), CV_64F);
//	
//}
int main()
{

	string name = path + "1Dose.jpg";
	string newImage = path + "2Dose.jpg";
	Mat sizeImage = imread(name);
	Mat originalImage = imread(newImage);
	Rect regionOfCan(0, 0, sizeImage.size().width, sizeImage.size().height);
	Mat cutImage = Mat::zeros(sizeImage.size(), sizeImage.type());
	cutImage = originalImage(regionOfCan);
    return 0;
}

