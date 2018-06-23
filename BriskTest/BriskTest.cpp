// BriskTest.cpp : Defines the entry point for the console application.
//

//sourcecode:
//https://github.com/oreillymedia/Learning-OpenCV-3_examples/blob/master/example_16-02.cpp

#include "stdafx.h"



#include "opencv2/highgui/highgui.hpp"

#include "opencv2/calib3d/calib3d.hpp"

#include "opencv2/imgproc/imgproc.hpp"

#include "opencv2/features2d.hpp"


#include <iostream>


#include <vector>

#include <iostream>

#include <cstdlib>

#include <fstream>

#include <algorithm>

#include <opencv2/opencv.hpp>

#include <opencv2/objdetect.hpp>

#include <opencv2/core/core.hpp>

#include <opencv2/highgui/highgui.hpp>

#include <opencv2/features2d.hpp>
//
//#include <opencv2/xfeatures2d.hpp>
//
//#include <opencv2/xfeatures2d/nonfree.hpp>

#include <opencv2/calib3d.hpp>

#include <opencv2/imgproc.hpp>

#include <opencv2/core/utility.hpp>

#include <opencv2/core/ocl.hpp>



using namespace cv;

using namespace std;

const double kDistanceCoef = 4.0;

const int kMaxMatchingSize = 50;



inline void detect_and_compute(string type, Mat& img, vector<KeyPoint>& kpts, Mat& desc) {

	if (type.find("fast") == 0) {

		type = type.substr(4);

		Ptr<FastFeatureDetector> detector = FastFeatureDetector::create(10, true);

		detector->detect(img, kpts);

	}

	if (type.find("blob") == 0) {

		type = type.substr(4);

		Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create();

		detector->detect(img, kpts);

	}



	if (type == "orb") {

		Ptr<ORB> orb = ORB::create();

		orb->detectAndCompute(img, Mat(), kpts, desc);

	}

	if (type == "brisk") {

		Ptr<BRISK> brisk = BRISK::create();

		brisk->detectAndCompute(img, Mat(), kpts, desc);

	}

	if (type == "kaze") {

		Ptr<KAZE> kaze = KAZE::create();

		kaze->detectAndCompute(img, Mat(), kpts, desc);

	}

	if (type == "akaze") {

		Ptr<AKAZE> akaze = AKAZE::create();

		akaze->detectAndCompute(img, Mat(), kpts, desc);

	}






}

inline void match(string type, Mat& desc1, Mat& desc2, vector<DMatch>& matches) {

	matches.clear();

	if (type == "bf") {

		BFMatcher desc_matcher(cv::NORM_L2, true);

		desc_matcher.match(desc1, desc2, matches, Mat());

	}

	if (type == "knn") {

		BFMatcher desc_matcher(cv::NORM_L2, true);

		vector< vector<DMatch> > vmatches;

		desc_matcher.knnMatch(desc1, desc2, vmatches, 1);

		for (int i = 0; i < static_cast<int>(vmatches.size()); ++i) {

			if (!vmatches[i].size()) {

				continue;

			}

			matches.push_back(vmatches[i][0]);

		}

	}

	std::sort(matches.begin(), matches.end());

	while (matches.front().distance * kDistanceCoef < matches.back().distance) {

		matches.pop_back();

	}

	while (matches.size() > kMaxMatchingSize) {

		matches.pop_back();

	}

}

inline void findKeyPointsHomography(vector<KeyPoint>& kpts1, vector<KeyPoint>& kpts2,

	vector<DMatch>& matches, vector<char>& match_mask) {

	if (static_cast<int>(match_mask.size()) < 3) {

		return;

	}

	vector<Point2f> pts1;

	vector<Point2f> pts2;

	for (int i = 0; i < static_cast<int>(matches.size()); ++i) {

		pts1.push_back(kpts1[matches[i].queryIdx].pt);

		pts2.push_back(kpts2[matches[i].trainIdx].pt);

	}

	findHomography(pts1, pts2, cv::RANSAC, 4, match_mask);

}

int main(int argc, char** argv) {



	string desc_type("brisk");

	string match_type("bf"); //bf or knn


	string path = "C:\\Users\\delia\\Desktop\\P4\\bver\\Bilder\\";
	string img_file1(path+"1Dose.jpg");

	string img_file2(path+"2Dose.jpg");



	Mat img1 = cv::imread(img_file1, CV_LOAD_IMAGE_COLOR);

	Mat img2 = cv::imread(img_file2, CV_LOAD_IMAGE_COLOR);



	if (img1.channels() != 1) {

		cvtColor(img1, img1, cv::COLOR_RGB2GRAY);

	}



	if (img2.channels() != 1) {

		cvtColor(img2, img2, cv::COLOR_RGB2GRAY);

	}



	vector<KeyPoint> kpts1;

	vector<KeyPoint> kpts2;



	Mat desc1;

	Mat desc2;



	vector<DMatch> matches;



	detect_and_compute(desc_type, img1, kpts1, desc1);

	detect_and_compute(desc_type, img2, kpts2, desc2);



	match(match_type, desc1, desc2, matches);



	vector<char> match_mask(matches.size(), 1);

	findKeyPointsHomography(kpts1, kpts2, matches, match_mask);



	Mat res;

	cv::drawMatches(img1, kpts1, img2, kpts2, matches, res, Scalar::all(-1),

		Scalar::all(-1), match_mask, DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);



	cv::imshow("result", res);

	cv::waitKey(0);



	return 0;

}


//
//static void help(char** argv)
//
//{
//
//	cout << "\nThis program demonstrats keypoint finding and matching between 2 images using features2d framework.\n"
//
//		<< "   In one case, the 2nd image is synthesized by homography from the first, in the second case, there are 2 images\n"
//
//		<< "\n"
//
//		<< "Case1: second image is obtained from the first (given) image using random generated homography matrix\n"
//
//		<< argv[0] << " [detectorType] [descriptorType] [matcherType] [matcherFilterType] [image] [evaluate(0 or 1)]\n"
//
//		<< "Example of case1:\n"
//
//		<< "./descriptor_extractor_matcher SURF SURF FlannBased NoneFilter cola.jpg 0\n"
//
//		<< "\n"
//
//		<< "Case2: both images are given. If ransacReprojThreshold>=0 then homography matrix are calculated\n"
//
//		<< argv[0] << " [detectorType] [descriptorType] [matcherType] [matcherFilterType] [image1] [image2] [ransacReprojThreshold]\n"
//
//		<< "\n"
//
//		<< "Matches are filtered using homography matrix in case1 and case2 (if ransacReprojThreshold>=0)\n"
//
//		<< "Example of case2:\n"
//
//		<< "./descriptor_extractor_matcher SURF SURF BruteForce CrossCheckFilter cola1.jpg cola2.jpg 3\n"
//
//		<< "\n"
//
//		<< "Possible detectorType values: see in documentation on createFeatureDetector().\n"
//
//		<< "Possible descriptorType values: see in documentation on createDescriptorExtractor().\n"
//
//		<< "Possible matcherType values: see in documentation on createDescriptorMatcher().\n"
//
//		<< "Possible matcherFilterType values: NoneFilter, CrossCheckFilter." << endl;
//
//}
//
//
//
//#define DRAW_RICH_KEYPOINTS_MODE     0
//
//#define DRAW_OUTLIERS_MODE           0
//
//
//
//const string winName = "correspondences";
//
//
//
//enum { NONE_FILTER = 0, CROSS_CHECK_FILTER = 1 };
//
//
//
//static int getMatcherFilterType(const string& str)
//
//{
//
//	if (str == "NoneFilter")
//
//		return NONE_FILTER;
//
//	if (str == "CrossCheckFilter")
//
//		return CROSS_CHECK_FILTER;
//
//	CV_Error(CV_StsBadArg, "Invalid filter name");
//
//	return -1;
//
//}
//
//
//
//static void simpleMatching(Ptr<DescriptorMatcher>& descriptorMatcher,
//
//	const Mat& descriptors1, const Mat& descriptors2,
//
//	vector<DMatch>& matches12)
//
//{
//
//	vector<DMatch> matches;
//
//	descriptorMatcher->match(descriptors1, descriptors2, matches12);
//
//}
//
//
//
//static void crossCheckMatching(Ptr<DescriptorMatcher>& descriptorMatcher,
//
//	const Mat& descriptors1, const Mat& descriptors2,
//
//	vector<DMatch>& filteredMatches12, int knn = 1)
//
//{
//
//	filteredMatches12.clear();
//
//	vector<vector<DMatch> > matches12, matches21;
//
//	descriptorMatcher->knnMatch(descriptors1, descriptors2, matches12, knn);
//
//	descriptorMatcher->knnMatch(descriptors2, descriptors1, matches21, knn);
//
//	for (size_t m = 0; m < matches12.size(); m++)
//
//	{
//
//		bool findCrossCheck = false;
//
//		for (size_t fk = 0; fk < matches12[m].size(); fk++)
//
//		{
//
//			DMatch forward = matches12[m][fk];
//
//
//
//			for (size_t bk = 0; bk < matches21[forward.trainIdx].size(); bk++)
//
//			{
//
//				DMatch backward = matches21[forward.trainIdx][bk];
//
//				if (backward.trainIdx == forward.queryIdx)
//
//				{
//
//					filteredMatches12.push_back(forward);
//
//					findCrossCheck = true;
//
//					break;
//
//				}
//
//			}
//
//			if (findCrossCheck) break;
//
//		}
//
//	}
//
//}
//
//
//
//static void warpPerspectiveRand(const Mat& src, Mat& dst, Mat& H, RNG& rng)
//
//{
//
//	H.create(3, 3, CV_32FC1);
//
//	H.at<float>(0, 0) = rng.uniform(0.8f, 1.2f);
//
//	H.at<float>(0, 1) = rng.uniform(-0.1f, 0.1f);
//
//	H.at<float>(0, 2) = rng.uniform(-0.1f, 0.1f)*src.cols;
//
//	H.at<float>(1, 0) = rng.uniform(-0.1f, 0.1f);
//
//	H.at<float>(1, 1) = rng.uniform(0.8f, 1.2f);
//
//	H.at<float>(1, 2) = rng.uniform(-0.1f, 0.1f)*src.rows;
//
//	H.at<float>(2, 0) = rng.uniform(-1e-4f, 1e-4f);
//
//	H.at<float>(2, 1) = rng.uniform(-1e-4f, 1e-4f);
//
//	H.at<float>(2, 2) = rng.uniform(0.8f, 1.2f);
//
//
//
//	warpPerspective(src, dst, H, src.size());
//
//}
//
//
//
//static void doIteration(const Mat& img1, Mat& img2, bool isWarpPerspective,
//
//	vector<KeyPoint>& keypoints1, const Mat& descriptors1,
//
//	Ptr<FeatureDetector>& detector, Ptr<DescriptorExtractor>& descriptorExtractor,
//
//	Ptr<DescriptorMatcher>& descriptorMatcher, int matcherFilter, bool eval,
//
//	double ransacReprojThreshold, RNG& rng)
//
//{
//
//	assert(!img1.empty());
//
//	Mat H12;
//
//	if (isWarpPerspective)
//
//		warpPerspectiveRand(img1, img2, H12, rng);
//
//	else
//
//		assert(!img2.empty()/* && img2.cols==img1.cols && img2.rows==img1.rows*/);
//
//
//
//	cout << endl << "< Extracting keypoints from second image..." << endl;
//
//	vector<KeyPoint> keypoints2;
//
//	detector->detect(img2, keypoints2);
//
//	cout << keypoints2.size() << " points" << endl << ">" << endl;
//
//
//
//	if (!H12.empty() && eval)
//
//	{
//
//		cout << "< Evaluate feature detector..." << endl;
//
//		float repeatability;
//
//		int correspCount;
//
//		evaluateFeatureDetector(img1, img2, H12, &keypoints1, &keypoints2, repeatability, correspCount);
//
//		cout << "repeatability = " << repeatability << endl;
//
//		cout << "correspCount = " << correspCount << endl;
//
//		cout << ">" << endl;
//
//	}
//
//
//
//	cout << "< Computing descriptors for keypoints from second image..." << endl;
//
//	Mat descriptors2;
//
//	descriptorExtractor->compute(img2, keypoints2, descriptors2);
//
//	cout << ">" << endl;
//
//
//
//	cout << "< Matching descriptors..." << endl;
//
//	vector<DMatch> filteredMatches;
//
//	switch (matcherFilter)
//
//	{
//
//	case CROSS_CHECK_FILTER:
//
//		crossCheckMatching(descriptorMatcher, descriptors1, descriptors2, filteredMatches, 1);
//
//		break;
//
//	default:
//
//		simpleMatching(descriptorMatcher, descriptors1, descriptors2, filteredMatches);
//
//	}
//
//	cout << ">" << endl;
//
//
//
//	if (!H12.empty() && eval)
//
//	{
//
//		cout << "< Evaluate descriptor matcher..." << endl;
//
//		vector<Point2f> curve;
//
//		Ptr<GenericDescriptorMatcher> gdm = new VectorDescriptorMatcher(descriptorExtractor, descriptorMatcher);
//
//		evaluateGenericDescriptorMatcher(img1, img2, H12, keypoints1, keypoints2, 0, 0, curve, gdm);
//
//
//
//		Point2f firstPoint = *curve.begin();
//
//		Point2f lastPoint = *curve.rbegin();
//
//		int prevPointIndex = -1;
//
//		cout << "1-precision = " << firstPoint.x << "; recall = " << firstPoint.y << endl;
//
//		for (float l_p = 0; l_p <= 1 + FLT_EPSILON; l_p += 0.05f)
//
//		{
//
//			int nearest = getNearestPoint(curve, l_p);
//
//			if (nearest >= 0)
//
//			{
//
//				Point2f curPoint = curve[nearest];
//
//				if (curPoint.x > firstPoint.x && curPoint.x < lastPoint.x && nearest != prevPointIndex)
//
//				{
//
//					cout << "1-precision = " << curPoint.x << "; recall = " << curPoint.y << endl;
//
//					prevPointIndex = nearest;
//
//				}
//
//			}
//
//		}
//
//		cout << "1-precision = " << lastPoint.x << "; recall = " << lastPoint.y << endl;
//
//		cout << ">" << endl;
//
//	}
//
//
//
//	vector<int> queryIdxs(filteredMatches.size()), trainIdxs(filteredMatches.size());
//
//	for (size_t i = 0; i < filteredMatches.size(); i++)
//
//	{
//
//		queryIdxs[i] = filteredMatches[i].queryIdx;
//
//		trainIdxs[i] = filteredMatches[i].trainIdx;
//
//	}
//
//
//
//	if (!isWarpPerspective && ransacReprojThreshold >= 0)
//
//	{
//
//		cout << "< Computing homography (RANSAC)..." << endl;
//
//		vector<Point2f> points1; KeyPoint::convert(keypoints1, points1, queryIdxs);
//
//		vector<Point2f> points2; KeyPoint::convert(keypoints2, points2, trainIdxs);
//
//		H12 = findHomography(Mat(points1), Mat(points2), CV_RANSAC, ransacReprojThreshold);
//
//		cout << ">" << endl;
//
//	}
//
//
//
//	Mat drawImg;
//
//	if (!H12.empty()) // filter outliers
//
//	{
//
//		vector<char> matchesMask(filteredMatches.size(), 0);
//
//		vector<Point2f> points1; KeyPoint::convert(keypoints1, points1, queryIdxs);
//
//		vector<Point2f> points2; KeyPoint::convert(keypoints2, points2, trainIdxs);
//
//		Mat points1t; perspectiveTransform(Mat(points1), points1t, H12);
//
//
//
//		double maxInlierDist = ransacReprojThreshold < 0 ? 3 : ransacReprojThreshold;
//
//		for (size_t i1 = 0; i1 < points1.size(); i1++)
//
//		{
//
//			if (norm(points2[i1] - points1t.at<Point2f>((int)i1, 0)) <= maxInlierDist) // inlier
//
//				matchesMask[i1] = 1;
//
//		}
//
//		// draw inliers
//
//		drawMatches(img1, keypoints1, img2, keypoints2, filteredMatches, drawImg, CV_RGB(0, 255, 0), CV_RGB(0, 0, 255), matchesMask
//
//#if DRAW_RICH_KEYPOINTS_MODE
//
//			, DrawMatchesFlags::DRAW_RICH_KEYPOINTS
//
//#endif
//
//		);
//
//
//
//#if DRAW_OUTLIERS_MODE
//
//		// draw outliers
//
//		for (size_t i1 = 0; i1 < matchesMask.size(); i1++)
//
//			matchesMask[i1] = !matchesMask[i1];
//
//		drawMatches(img1, keypoints1, img2, keypoints2, filteredMatches, drawImg, CV_RGB(0, 0, 255), CV_RGB(255, 0, 0), matchesMask,
//
//			DrawMatchesFlags::DRAW_OVER_OUTIMG | DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
//
//#endif
//
//
//
//		cout << "Number of inliers: " << countNonZero(matchesMask) << endl;
//
//	}
//
//	else
//
//		drawMatches(img1, keypoints1, img2, keypoints2, filteredMatches, drawImg);
//
//
//
//	imshow(winName, drawImg);
//
//}
//
//
//
//
//
//int main(int argc, char** argv)
//
//{
//
//	if (argc != 7 && argc != 8)
//
//	{
//
//		help(argv);
//
//		return -1;
//
//	}
//
//
//
//	//cv::initModule_nonfree();
//
//
//
//	bool isWarpPerspective = argc == 7;
//
//	double ransacReprojThreshold = -1;
//
//	if (!isWarpPerspective)
//
//		ransacReprojThreshold = atof(argv[7]);
//
//
//
//	cout << "< Creating detector, descriptor extractor and descriptor matcher ..." << endl;
//	string path = "C:\\Users\\delia\\Desktop\\P4\\bver\\Bilder\\";
//	string name = path +"1Dose.jpg";
//	Mat first = imread(name);
//
//	Ptr<FeatureDetector> detector =  FastFeatureDetector::create(); //not sure if here really fast or something else ? FeatureDetector::create(argv[1]);
//
//	Ptr<DescriptorExtractor> descriptorExtractor = BRISK::create();
//	
//
//	Ptr<DescriptorMatcher> descriptorMatcher = DescriptorMatcher::create(argv[3]);
//
//	int mactherFilterType = getMatcherFilterType(argv[4]);
//
//	bool eval = !isWarpPerspective ? false : (atoi(argv[6]) == 0 ? false : true);
//
//	cout << ">" << endl;
//
//	if (detector.empty() || descriptorExtractor.empty() || descriptorMatcher.empty())
//
//	{
//
//		cout << "Can not create detector or descriptor exstractor or descriptor matcher of given types" << endl;
//
//		return -1;
//
//	}
//
//
//
//	cout << "< Reading the images..." << endl;
//
//	Mat img1 = imread(argv[5]), img2;
//
//	if (!isWarpPerspective)
//
//		img2 = imread(argv[6]);
//
//	cout << ">" << endl;
//
//	if (img1.empty() || (!isWarpPerspective && img2.empty()))
//
//	{
//
//		cout << "Can not read images" << endl;
//
//		return -1;
//
//	}
//
//
//
//	cout << endl << "< Extracting keypoints from first image..." << endl;
//
//	vector<KeyPoint> keypoints1;
//
//	detector->detect(img1, keypoints1);
//
//	cout << keypoints1.size() << " points" << endl << ">" << endl;
//
//
//
//	cout << "< Computing descriptors for keypoints from first image..." << endl;
//
//	Mat descriptors1;
//
//	descriptorExtractor->compute(img1, keypoints1, descriptors1);
//
//	cout << ">" << endl;
//
//
//
//	namedWindow(winName, 1);
//
//	RNG rng = theRNG();
//
//	doIteration(img1, img2, isWarpPerspective, keypoints1, descriptors1,
//
//		detector, descriptorExtractor, descriptorMatcher, mactherFilterType, eval,
//
//		ransacReprojThreshold, rng);
//
//	for (;;)
//
//	{
//
//		char c = (char)waitKey(0);
//
//		if (c == '\x1b') // esc
//
//		{
//
//			cout << "Exiting ..." << endl;
//
//			break;
//
//		}
//
//		else if (isWarpPerspective)
//
//		{
//
//			doIteration(img1, img2, isWarpPerspective, keypoints1, descriptors1,
//
//				detector, descriptorExtractor, descriptorMatcher, mactherFilterType, eval,
//
//				ransacReprojThreshold, rng);
//
//		}
//
//	}
//
//	return 0;
//
//}
