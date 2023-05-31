#ifndef FIRSTSEGMENTATOR_H
#define FIRSTSEGMENTATOR_H

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"


cv::Mat firstSegmentationFunc(cv::Mat img);
bool compareCircles(const cv::Vec3f& circ1, const cv::Vec3f& circ2);
bool isOverlap(const cv::Vec3f& circ1, const cv::Vec3f& circ2);
std::vector<cv::Vec3f> removeInnerCircles(const std::vector<cv::Vec3f>& circles);

#endif