#ifndef PREPTESTER_H
#define PREPTESTER_H

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/types.hpp>

static void on_trackbar(int, void*);
cv::Mat testPreProcessing(cv::Mat img);
std::vector<cv::Mat> multipleTestPreProcessing(std::vector<cv::Mat> imgVector);

#endif