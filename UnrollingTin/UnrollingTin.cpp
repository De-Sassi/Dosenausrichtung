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
#include <opencv2\imgproc.hpp>

//brisk
#include  <opencv2/features2d.hpp>
//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>


using namespace cv;
using namespace std;


//adjust the path to the images save point
//string path = "C:\\Users\\delia\\Desktop\\P4\\bver\\Bilder\\";
//string path = "C:\\Users\\delia\\Desktop\\P4\\Dosenbilder22\\"; 
string path = "C:\\Users\\delia\\Desktop\\P4\\Dosenbilder23\\beer_WOB\\";


//loads the image from memory and rescales it
void loadImage(string imageName, Mat *loadedImage)
{
	string name =path+imageName;
	*loadedImage = imread(name);
	resize(*loadedImage, *loadedImage, Size(), 0.5, 0.5, CV_INTER_AREA);

	//Show
	cv::namedWindow("normal", WINDOW_AUTOSIZE);
	imshow("normal", *loadedImage);
}


////creates the Mask with help from the background picture. 
//void createMask(Mat *image, Mat *mask)
//{
//	//Blurres for better Edge detection. This way small elements are not detected as an edge
//	Mat blurred;
//	blur(*image, blurred, Size(4, 4));
//
//	//load picture with only background without tin
//	string background = path + "background.jpg";
//	Mat backgroundPicture = imread(background);
//	resize(backgroundPicture, backgroundPicture, Size(), 0.5, 0.5, CV_INTER_AREA);
//	blur(backgroundPicture, backgroundPicture, Size(4, 4));
//
//	Ptr<BackgroundSubtractor> pMOG2;
//	//Create Background Substractor objects
//	pMOG2 = createBackgroundSubtractorMOG2();
//	pMOG2->apply(backgroundPicture, *mask);
//	pMOG2->apply(*image, *mask);
//	//Creates an black and white image instead of black and gray (8Bit image!)
//	normalize(*mask, *mask, 0.0, 255.0, CV_MINMAX, CV_8UC1);
//
//	cv::namedWindow("mask", WINDOW_AUTOSIZE);
//	imshow("mask", *mask);
//}

//
////Detctes the edges from the mask.
//void edgeDetection(Mat *mask,vector<vector<Point>> *contours)
//{
//	int lowThreshold = 10;
//	int ratio = 3;
//	int kernel_size = 3;
//
//	Mat edges;
//	Canny(*mask, edges, lowThreshold, lowThreshold*ratio, kernel_size);
//
//	cv::namedWindow("edges", WINDOW_AUTOSIZE);
//	imshow("edges", edges);
//
//	vector<Vec4i> hierarchy;
//	findContours(edges, *contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
//
//	imwrite(path + "edge.jpg", edges);
//
//}
//
//struct sortByX {
//	bool operator() (cv::Point pt1, cv::Point pt2) { return (pt1.x < pt2.x); }
//} sortByXAscending;
//
////The two biggest contours are the elements that surround the tin-> the tin has to be in between
//void get2BiggestContours(vector<vector<Point>> *contours, vector<Point> *biggest, vector<Point> *second)
//{
//	vector<std::tuple<int, vector<Point>>> contoursBySize;
//	int anzahlContours = 0;
//	//make rectangle around contour so it can be localized
//	for each (vector<Point> element in *contours)
//	{
//		Rect rect = boundingRect(element);
//		int size = rect.area();
//		std::tuple<int, vector<Point>> x = std::make_tuple(size, element);
//		contoursBySize.push_back(x);
//		anzahlContours++;
//	}
//	sort(begin(contoursBySize), end(contoursBySize),
//		[](auto const &t1, auto const &t2) {
//		return get<0>(t1) > get<0>(t2); });
//
//	*biggest = get<1>(contoursBySize.front());
//	*second = get<1>(contoursBySize.at(1));
//}
//
//Point getBiggestX(vector<Point> *contour)
//{
//	sort(begin(*contour), end(*contour), sortByXAscending);
//	Point biggest = contour->back();
//	return biggest;
//
//}

//Point getSmallestX(vector<Point> *contour)
//{
//	sort(begin(*contour), end(*contour), sortByXAscending);
//	Point smallest = contour->front();
//	return smallest;
//}

////gets the left and right border of the tinimage with help of the contours from the mask
//void getTinBorders(vector<vector<Point>> *contours, Point *leftUpper, Point *rightUpper)
//{
//	vector<Point> biggest, second;
//	get2BiggestContours(contours, &biggest, &second);
//	Rect rectBiggest = boundingRect(biggest);
//	Point centerBiggest=(rectBiggest.br()+rectBiggest.tl())*0.5;
//	Rect rectSecond = boundingRect(second);
//	Point centerSecond = (rectSecond.br() + rectSecond.tl())*0.5;
//
//	vector<Point> left = (centerBiggest.x < centerSecond.x) ? biggest : second;
//	vector<Point> right = (centerBiggest.x > centerSecond.x) ? biggest : second;
//	//leftUpperCorner
//	*leftUpper = getBiggestX(&left);
//	//right upperCorner
//	*rightUpper = getSmallestX(&right);
//
//}

bool sortByFirstX(const Vec4i &lhs, const Vec4i &rhs)
{
	return lhs[0] < rhs[0];
}

bool isHorizontal(Vec4i line)
{
	int firstX = line[0];
	int firstY = line[1];
	int secondX = line[2];
	int secondY = line[3];

	int width = abs(firstX - secondX);
	int height = abs(firstY - secondY);
	if (width > height)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool isVertical(Vec4i line)
{
	int firstX = line[0];
	int firstY = line[1];
	int secondX = line[2];
	int secondY = line[3];

	int width = abs(firstX - secondX);
	int height = abs(firstY - secondY);
	if (width < height)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//gets the left and right border of the tinimage 
void getTinBorders(Mat *image, Vec4i *leftUpper, Vec4i *rightUpper)
{
	Mat dst, cdst;
	Canny(*image, dst, 50, 200, 3);
	cvtColor(dst, cdst, COLOR_GRAY2BGR);

	vector<Vec4i> lines;

	HoughLinesP(dst, lines, 1, CV_PI / 180, 50, 50, 10);
	vector<Vec4i> verticalLines;
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		if (isVertical(l))
		{
			//Blue
			line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, 2);
			verticalLines.push_back(l);
		}
	}
	//sortiert nach x. grösste x zuerst
	sort(begin(verticalLines), end(verticalLines), sortByFirstX);
	*leftUpper = verticalLines.front();
	*rightUpper = verticalLines.back();
}

//cut Images based on the borders of the tin
void cutImage(Mat *image, Mat *cutImage/*, vector<vector<Point>> *contours*/)
{
	Vec4i leftUpper;
	Vec4i rightUpper;

	getTinBorders(image,&leftUpper,&rightUpper);
	
	//CropedImage= Only Image with can in it
	Rect regionOfCan(leftUpper[0], 0, rightUpper[0] - leftUpper[0], (*image).size().height);
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


//void remapInWorldcoordinates(Mat *image, Mat *flattedImage)
//{
//
//	//does everything need to be in pixelmeasure? or not?
//
//
//	//vom Bilddarstellung in Worldkoordinaten
//	float focal_length = 9; //in mm; aus spezifikation mit Objektiv (Fujinon 1:1.4/9mm HF9HA-1B
//	float pixel_size =(float)4.65*pow(10,-3); //in mm; Kamera DFW-X710 Sony 
//	float f = focal_length/pixel_size;
//	float h = 99;//in mm Höhe Kamera mitte
//	float l = 253;//in mm abstand von kamera bis LInse (müsste doch mit Chip sein nicht ?)
//	
//	//mm->pixel
//	h = h / pixel_size;
//	l = l / pixel_size;
//
//	int H_x = image->size().height/2;
//	int H_y = image->size().width / 2;
//
//
//	int radius = image->size().width/2;
//	int widthfinalImage=(int)(radius * M_PI);
//	//Width final image -> 2*r*pi
//	float c_x = widthfinalImage /2; //for shift of point zero
//	float c_y = image->size().height;
//
//	*flattedImage = Mat::zeros(Size(widthfinalImage, image->rows), image->type());
//	Mat mapx, mapy;
//	mapy.create(flattedImage->size(), CV_32FC1);
//	mapx.create(flattedImage->size(), CV_32FC1);
//
//	for (int j = 0; j < image->rows; j++)//j=y
//	{
//		for (int i = 0; i < image->cols; i++)//i=x
//		{
//
//			//World coordinates
//			float testsin = sin((c_x - i) / radius);
//			float P_w_x = radius*testsin-c_x;
//			float P_w_y = j-c_y; //evt c_y -j da ja y in postive richtug
//			float testcos = cos((c_x - i) / radius);
//			float P_w_z = -radius*testcos;
//			//Camera cooridnates
//			float P_c_x = P_w_x;
//			float P_c_y = P_w_y + h;
//			float P_c_z = P_w_z + l;
//			//homogene image coordinates;
//			float b_x = f*P_c_x / P_c_z + H_x;
//			float b_y = f*P_c_y / P_c_z + H_y;
//
//			mapx.at<float>(j, i) = b_x;
//			mapy.at<float>(j, i) = b_y;
//		}
//	}
//
//	remap(*image, *flattedImage, mapx, mapy, CV_INTER_LINEAR);
//
//	cv::namedWindow("flat", WINDOW_AUTOSIZE);
//	imshow("flat", *flattedImage);
//
//
//}


int main()
{
	Mat  image,cutedImage,flattedImage;


	
	//vector<vector<Point>> contours;
	for (int i = 2; i <= 9; i++)
	{
		loadImage("wob"+ to_string(i)+".jpg", &image);
		cutImage(&image, &cutedImage);
	/*	remapToFlat(&cutedImage, &flattedImage);*/
		imwrite(path+"cutted\\" + to_string(i) + "cut.jpg", cutedImage);
		//remapInWorldcoordinates(&cutedImage, &flattedImage);
	}

	waitKey(0);
	destroyAllWindows();
	return EXIT_SUCCESS;
}

