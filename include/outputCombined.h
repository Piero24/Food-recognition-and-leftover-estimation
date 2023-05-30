#ifndef OUTPUTCOMBINED_H
#define OUTPUTCOMBINED_H

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/types.hpp>

cv::Mat pushOutTray(std::vector<cv::Mat> hcombinedVec);

#endif