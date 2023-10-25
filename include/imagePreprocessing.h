#ifndef IMAGEPREPROCESSING_H
#define IMAGEPREPROCESSING_H

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/types.hpp>

/**
* Performs a series of preprocessing operations on the image for segmentation.
* @param img The input image to process.
* @return The processed image after preprocessing operations.
*/
cv::Mat segmentationPreprocessing(cv::Mat img);
/**
* Gets image dimensions and calculates area.
* @param img The input image to calculate the size of.
* @return A vector containing the width, height, and area of the image.
*/
std::vector<int> imgDimensions(cv::Mat img);
/**
* Performs Canny edge detection on the input image and outputs the image
* with detected contours overlaid.
* @param img The input image to perform Canny edge detection on.
* @return The image with detected edges overlaid.
*/
cv::Mat cannyPreprocessing(cv::Mat img);
/**
* Removes white regions from the input image and returns the resulting image.
* @param img The input image to remove white regions from.
* @return The resulting image without the white regions.
*/
cv::Mat removeWhite(cv::Mat img);
/**
* Removes blue regions from the input image and returns the resulting image.
* @param img The input image to remove the blue regions from.
* @return The resulting image without the blue regions.
*/
cv::Mat removeBlue(cv::Mat img);
/**
* Removes post-it-like tone regions from the input image and outputs the resulting image.
* @param img The input image to remove post-it-like regions from.
* @return The resulting image without the post-it-like regions.
*/
cv::Mat removePostit(cv::Mat img);
/**
* Performs dilation on the input image using a specific elliptical kernel and returns
* the resulting image.
* @param img The input image to dilate on.
* @return The resulting image after dilation.
*/
cv::Mat imageDilate(cv::Mat img);

#endif
