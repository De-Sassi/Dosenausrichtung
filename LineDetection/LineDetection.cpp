// LineDetection.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"



#include <opencv2/highgui.hpp>
#include <opencv2\imgproc.hpp>

#include <iostream>

using namespace cv;
using namespace std;

bool isVertical(Vec4i);
bool isHorizontal(Vec4i);

bool sortByFirstX(const Vec4i &lhs, const Vec4i &rhs)
{
	return lhs[0] < rhs[0];
}
int DetectLines(const char* filename, const char* sourceName, const char* destName)
{
	Mat src = imread(filename, 0);
	if (src.empty())
	{
		cout << "can not open " << filename << endl;
		return -1;
	}

	Mat dst, cdst;
	Canny(src, dst, 50, 200, 3);
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
		//if (isHorizontal(l))
		//{
		//	//Green
		//	line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 0), 3, 2);
		//}
		//if (!isHorizontal(l) && !isVertical(l))
		//{
		//	//RED
		//	line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, 2);
		//}
	}

	//sortiert nach x. grösste x zuerst
	sort(begin(verticalLines),end(verticalLines), sortByFirstX);
	Vec4i left =verticalLines.front();
	Vec4i right = verticalLines.back();
	//RED
	line(cdst, Point(left[0], left[1]), Point(left[2], left[3]), Scalar(0, 0, 255), 3, 2);
	//GREEN
	line(cdst, Point(right[0], right[1]), Point(right[2], right[3]), Scalar(0, 255, 0), 3, 2);
	imshow(sourceName, src);
	imshow(destName, cdst);

	return 0;
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

int main(int argc, char** argv)
{
	const string path = "C:\\Users\\delia\\Desktop\\P4\\Dosenbilder23\\beer_WOB\\1.jpg";
	DetectLines("C:\\Users\\delia\\Desktop\\P4\\Dosenbilder23\\beer_WOB\\wob1.jpg", "line src1", "line dest1");
	DetectLines("C:\\Users\\delia\\Desktop\\P4\\Dosenbilder23\\beer_WOB\\wob3.jpg", "line src2", "line dest2");
	DetectLines("C:\\Users\\delia\\Desktop\\P4\\Dosenbilder23\\beer_WOB\\wob16.jpg", "line src16", "line dest16");
	//DetectLines("..\\2.jpg", "door src", "door dest");
	waitKey(0);

	return 0;
}

//tests um nur dose auszulesen

//vector<vector<Point>> contours;
//loadImage("wob1.jpg");
//Mat test;
//cvtColor(image, test, CV_BGR2GRAY);
//cv::namedWindow("Gray", WINDOW_AUTOSIZE);
//imshow("Gray", test);
//
//int ddepth = CV_16S;
//Mat grad_x, grad_y, grad;
//Sobel(test, grad_x, ddepth, 1, 0);
//Sobel(test, grad_y, ddepth, 0, 1);
//
//
//Mat abs_grad_x, abs_grad_y;
//// converting back to CV_8U
//convertScaleAbs(grad_x, abs_grad_x);
//convertScaleAbs(grad_y, abs_grad_y);
//addWeighted(abs_grad_x, 0.1, abs_grad_y, 0.1, 0, grad);
//cv::namedWindow("test", WINDOW_AUTOSIZE);
//imshow("test", grad);
//
//edgeDetection(&grad, &contours);
//for each (std::vector<Point> element in contours)
//{
//	std::vector<Point> approx;
//	double peri = arcLength(element, true);
//	approxPolyDP(element, approx, 0.02*peri, true);
//	std::vector<std::vector<Point>> test = { approx };
//	drawContours(grad, test, -1, (255, 255, 0), 2);
//
//}
//cv::namedWindow("draw", WINDOW_AUTOSIZE);
//imshow("draw", grad);
//cutImage(&image, &cutedImage, &contours);

