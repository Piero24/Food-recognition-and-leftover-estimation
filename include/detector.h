#ifndef DETECTOR_H
#define DETECTOR_H

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

class Detector {
private:
    cv::Mat img;
    std::vector<cv::Rect> rectanglesVector;
    std::vector<std::vector<std::vector<cv::Point>>> contours;

public:
	Detector() {};
    Detector(cv::Mat& image, std::vector<cv::Rect>& rects, std::vector<std::vector<std::vector<cv::Point>>>& conts)
    {img = image; rectanglesVector = rects; contours= conts;};
	/*Detector(cv::Mat image, std::vector<cv::Rect> rects, std::vector<std::vector<cv::Point>> conts)
    : img(image), rectanglesVector(rects), contours(conts) {};*/
    const cv::Mat getImage() const;
    const std::vector<cv::Rect> getRectanglesVector() const;
    const std::vector<std::vector<std::vector<cv::Point>>> getContours() const;
    Detector subjectIsolator(cv::Mat img, std::vector<cv::Vec3f>& circlesVector, std::vector<cv::Rect>& rectanglesVector);
    std::vector<cv::Rect> findRectanglesOutsideCircles(const std::vector<cv::Rect>& rectanglesVector, const std::vector<cv::Vec3f>& circlesVector);
	cv::Mat noWhiteOnImg(cv::Mat img, int value1, int value2);
	cv::Mat test_2(cv::Mat img);
	cv::Rect rightRectangles(cv::Mat img);
	std::vector<std::vector<cv::Point>> rightContours(cv::Rect boundingRect, cv::Mat img);
    std::vector<cv::Rect> fromSegmentationToBBox(cv::Mat img, Detector detectorVec, int numOfBoxes);
};

#endif


