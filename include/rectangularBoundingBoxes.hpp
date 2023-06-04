#ifndef RECTANGULARBOUNDINGBOXES_H
#define RECTANGULARBOUNDINGBOXES_H

#include <iostream>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

// Green dot:
//     Unicode code: \U0001F7E2
//     RGB value: (0, 255, 0)
//     BGR value: (0, 255, 0)

// Red dot:
//     Unicode code: \U0001F534
//     RGB value: (255, 0, 0)
//     BGR value: (0, 0, 255)


/**
* Finds rectangular bounding boxes in the given image.
* @param img The input image.
* @param img2 The cloned image.
* @return A vector of cv::Rect representing the rectangular bounding boxes found in the image.
*/
std::vector<cv::Rect> findRectangularBoundingBoxes(cv::Mat img, cv::Mat img2);
/**
* Compares two rectangles based on their areas.
* @param rect1 The first rectangle.
* @param rect2 The second rectangle.
* @return True if the area of rect1 is greater than the area of rect2, false otherwise.
*/
bool compareRectangles(const cv::Rect& rect1, const cv::Rect& rect2);
/**
* Checks if two rectangles overlap.
* @param rect1 The first rectangle.
* @param rect2 The second rectangle.
* @return True if the rectangles overlap, false otherwise.
*/
bool isOverlap(const cv::Rect& rect1, const cv::Rect& rect2);
/**
* Removes inner rectangles from a vector of rectangles.
* @param rectangles The vector of rectangles.
* @return A new vector of rectangles with inner rectangles removed.
*/
std::vector<cv::Rect> removeInnerRectangles(const std::vector<cv::Rect>& rectangles);
/**
* Filters out rectangles based on their area.
* @param rectangles The vector of rectangles.
* @return A new vector of rectangles with rectangles outside the specified area range removed.
*/
std::vector<cv::Rect> pushOut(const std::vector<cv::Rect>& rectangles);
/**
* Performs region-based segmentation using the Selective Search algorithm.
* @param img The input image.
* @param quality The quality mode for Selective Search ("f" for fast, any other value for quality).
* @return A vector of rectangles representing the bounding boxes of the segmented regions.
*/
std::vector<cv::Rect> recSegmentation(cv::Mat img, std::string quality);


#endif