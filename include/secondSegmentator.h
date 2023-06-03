#ifndef SECONDSEGMENTATOR_H
#define SECONDSEGMENTATOR_H

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"


std::vector<cv::Rect> secondSegmentationFunc(cv::Mat img, cv::Mat img2);
bool compareRectangles(const cv::Rect& rect1, const cv::Rect& rect2);
// Funzione per controllare se due rettangoli si sovrappongono
bool isOverlap(const cv::Rect& rect1, const cv::Rect& rect2);
// Funzione per rimuovere i rettangoli interni
std::vector<cv::Rect> removeInnerRectangles(const std::vector<cv::Rect>& rectangles);
std::vector<cv::Rect> pushOut(const std::vector<cv::Rect>& rectangles);
cv::Mat allRectangles(cv::Mat img, std::vector<cv::Rect> rects);
std::vector<cv::Rect> recSegmentation(cv::Mat img, std::string quality);


#endif