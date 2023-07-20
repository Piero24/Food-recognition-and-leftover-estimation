#ifndef MATCHIMAGE_H
#define MATCHIMAGE_H

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/types.hpp>

void imgMatching(std::vector<cv::Mat> imgM, std::vector<cv::Mat>& imgL, std::vector<std::string>& imageNames, int n);
std::vector<cv::Mat> loadImage(std::string pathPatch, std::vector<std::string>& imageNames);

#endif
