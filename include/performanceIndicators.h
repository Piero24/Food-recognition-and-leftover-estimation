#ifndef PERFORMANCEINDICATOR_H
#define PERFORMANCEINDICATOR_H

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

// https://learnopencv.com/mean-average-precision-map-object-detection-model-evaluation-metric/
// https://towardsdatascience.com/metrics-to-evaluate-your-semantic-segmentation-model-6bcb99639aa2

/**
*
* @param img
* @param mask
* @return
*/
double meanAveragePrecision(cv::Mat img, cv::Mat mask);
/**
*
* @param img
* @param mask
* @return
*/
double meanIntersectionOverUnion(cv::Mat img, cv::Mat mask);
/**
*
* @param img
* @param mask
* @return
*/
double leftoverEstimator(cv::Mat img, cv::Mat mask);



#endif