#ifndef DRAWERANDTESTER_H
#define DRAWERANDTESTER_H

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include <boost/filesystem.hpp>

/**
* Draw circles and rectangles on the input image to test bounding boxes and circles.
* @param img The input image to draw the shapes on.
* @param circlesVector A vector of circles to draw on the image.
* @param rectanglesVector A vector of rectangles to draw on the image.
* @return An image with circles and rectangles drawn.
*/
cv::Mat boundingBoxTester(cv::Mat img, std::vector<cv::Vec3f> circlesVector, std::vector<cv::Rect> rectanglesVector);
/**
* Draws outlines and/or bounding boxes of the specified regions on the input image.
* @param img The input image on which to draw the outlines and/or bounding boxes.
* @param VecFoodDetected A vector of foods detected in the image.
* @param drawContour A flag indicating whether to draw contours.
* @param drawBoundingBox A flag indicating whether to draw bounding boxes.
* @return An image with outlines and/or bounding boxes drawn.
*/
cv::Mat contourAndBBoxDraw(cv::Mat img, std::vector<Food>& VecFoodDetected, bool drawContour = true, bool drawBoundingBox = true);
/**
* Overlay masks over images of food and leftovers for each tray, saving the resulting images.
* @param inputDir The directory containing the original images and input masks.
* @param outputDir The directory to save the overlay images.
*/
void overlayMasks(const std::string& inputDir, const std::string& outputDir);

#endif