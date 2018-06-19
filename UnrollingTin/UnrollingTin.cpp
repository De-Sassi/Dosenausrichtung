// UnrollingTin.cpp : Defines the entry point for the console application.
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


Mat image;
//adjust the path to the images save point
string path = "C:\\Users\\delia\\Desktop\\P4\\bver\\Bilder\\";


//loads the image from memory and rescales it
void loadImage(string imageName)
{
	string name = path + imageName;// "1Dose.jpg";
	image = imread(name);
	resize(image, image, Size(), 0.5, 0.5, CV_INTER_AREA);

	//Show
	cv::namedWindow("normal", WINDOW_AUTOSIZE);
	imshow("normal", image);
}


//creates the Mask with help from the background picture. 
void createMask(Mat *image, Mat *mask)
{
	//Blurres for better Edge detection. This way small elements are not detected as an edge
	Mat blurred;
	blur(*image, blurred, Size(4, 4));

	//load picture with only background without tin
	string background = path + "background.jpg";
	Mat backgroundPicture = imread(background);
	resize(backgroundPicture, backgroundPicture, Size(), 0.5, 0.5, CV_INTER_AREA);
	blur(backgroundPicture, backgroundPicture, Size(4, 4));

	Ptr<BackgroundSubtractor> pMOG2;
	//Create Background Substractor objects
	pMOG2 = createBackgroundSubtractorMOG2();
	pMOG2->apply(backgroundPicture, *mask);
	pMOG2->apply(*image, *mask);
	//Creates an black and white image instead of black and gray (8Bit image!)
	normalize(*mask, *mask, 0.0, 255.0, CV_MINMAX, CV_8UC1);

	cv::namedWindow("mask", WINDOW_AUTOSIZE);
	imshow("mask", *mask);
}


//Detctes the edges from the mask.
void edgeDetection(Mat *mask,vector<vector<Point>> *contours)
{
	int lowThreshold = 10;
	int ratio = 3;
	int kernel_size = 3;

	Mat edges;
	Canny(*mask, edges, lowThreshold, lowThreshold*ratio, kernel_size);

	cv::namedWindow("edges", WINDOW_AUTOSIZE);
	imshow("edges", edges);

	vector<Vec4i> hierarchy;
	findContours(edges, *contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

}

struct sortByX {
	bool operator() (cv::Point pt1, cv::Point pt2) { return (pt1.x < pt2.x); }
} sortByXAscending;

//The two biggest contours are the elements that surround the tin-> the tin has to be in between
void get2BiggestContours(vector<vector<Point>> *contours, vector<Point> *biggest, vector<Point> *second)
{
	vector<std::tuple<int, vector<Point>>> contoursBySize;
	int anzahlContours = 0;
	//make rectangle around contour so it can be localized
	for each (vector<Point> element in *contours)
	{
		Rect rect = boundingRect(element);
		int size = rect.area();
		std::tuple<int, vector<Point>> x = std::make_tuple(size, element);
		contoursBySize.push_back(x);
		anzahlContours++;
	}
	sort(begin(contoursBySize), end(contoursBySize),
		[](auto const &t1, auto const &t2) {
		return get<0>(t1) > get<0>(t2); });

	*biggest = get<1>(contoursBySize.front());
	*second = get<1>(contoursBySize.at(1));
}

Point getBiggestX(vector<Point> *contour)
{
	sort(begin(*contour), end(*contour), sortByXAscending);
	Point biggest = contour->back();
	return biggest;

}

Point getSmallestX(vector<Point> *contour)
{
	sort(begin(*contour), end(*contour), sortByXAscending);
	Point smallest = contour->front();
	return smallest;
}

//gets the left and right border of the tinimage with help of the contours from the mask
void getTinBorders(vector<vector<Point>> *contours, Point *leftUpper, Point *rightUpper)
{
	vector<Point> biggest, second;
	get2BiggestContours(contours, &biggest, &second);
	Rect rectBiggest = boundingRect(biggest);
	Point centerBiggest=(rectBiggest.br()+rectBiggest.tl())*0.5;
	Rect rectSecond = boundingRect(second);
	Point centerSecond = (rectSecond.br() + rectSecond.tl())*0.5;

	vector<Point> left = (centerBiggest.x < centerSecond.x) ? biggest : second;
	vector<Point> right = (centerBiggest.x > centerSecond.x) ? biggest : second;
	//leftUpperCorner
	*leftUpper = getBiggestX(&left);
	//right upperCorner
	*rightUpper = getSmallestX(&right);

}

//cut Images based on the borders of the tin
void cutImage(Mat *image, Mat *cutImage, vector<vector<Point>> *contours)
{
	Point leftUpper;
	Point rightUpper;

	getTinBorders(contours,&leftUpper,&rightUpper);
	
	//CropedImage= Only Image with can in it
	Rect regionOfCan(leftUpper.x, 0, rightUpper.x - leftUpper.x, (*image).size().height);
	*cutImage = (*image)(regionOfCan);
	cv::namedWindow("cut", WINDOW_AUTOSIZE);
	imshow("cut", *cutImage);

}
//Expects the Image of a Tin. This tin gets flated with a transformation. But only x-Axis is considered
void remapToFlat(Mat *image, Mat *flattedImage)
{
	
	int widthOriginal = image->size().width;
	int radius = widthOriginal / 2;

	Mat withoutEdge = (*image);
	*flattedImage = Mat::zeros(withoutEdge.size().height, widthOriginal, withoutEdge.type());

	Mat mapx, mapy;
	mapy.create(withoutEdge.size(), CV_32FC1);
	mapx.create(withoutEdge.size(), CV_32FC1);

	//afterwards for move the center to the middle
	float c_x = (float)withoutEdge.cols / 2;
	
	for (int j = 0; j < withoutEdge.rows; j++)//j=y
	{
		for (int i = 0; i < withoutEdge.cols; i++)//i=x
		{
			//New x is the arc length distanze from the center
			mapx.at<float>(j, i) = (float)c_x - radius*sin((c_x-i) / radius);
			mapy.at<float>(j, i) = (float)j;
		}
	}

	remap(withoutEdge, *flattedImage, mapx, mapy, CV_INTER_LINEAR);

	cv::namedWindow("flat", WINDOW_AUTOSIZE);
	imshow("flat", *flattedImage);
}



int main()
{
	Mat mask, cutedImage,flattedImage;
	
	vector<vector<Point>> contours;
	for(int i = 1; i <= 13; i++)
	{
		string number = to_string(i);
		loadImage(number+"Dose.jpg");
		createMask(&image, &mask);
		edgeDetection(&mask, &contours);
		cutImage(&image, &cutedImage, &contours);
		remapToFlat(&cutedImage, &flattedImage);
		
		imwrite(path+"flatted\\" + number +"flatNew.jpg", flattedImage);
	}


	waitKey(0);
	destroyAllWindows();
	return EXIT_SUCCESS;
}

