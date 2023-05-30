#ifndef IMAGEPREPROCESSING_H
#define IMAGEPREPROCESSING_H

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/types.hpp>

cv::Mat segmentationPreprocessing(cv::Mat img);
std::vector<int> imgDimensions(cv::Mat img);
cv::Mat removeBlue(cv::Mat img);
cv::Mat lightIncreaser(cv::Mat img);
cv::Mat cannyPreprocessing(cv::Mat img, int alpha1, int alpha2);
cv::Mat removeColor(cv::Mat img, cv::Scalar lowerBound, cv::Scalar upperBound);
cv::Mat imageDilate(cv::Mat img, int kernelSize);

#endif