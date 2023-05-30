#ifndef FIRSTSEGMENTATOR_H
#define FIRSTSEGMENTATOR_H

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"


cv::Mat firstSegmentationFunc(cv::Mat img, std::vector<cv::Rect> rects);

#endif