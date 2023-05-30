#ifndef UPLOADIMAGE_H
#define UPLOADIMAGE_H

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/types.hpp>

std::vector<cv::Mat> imgUploader(int argc, char** argv);

#endif