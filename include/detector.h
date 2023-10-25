#ifndef DETECTOR_H
#define DETECTOR_H

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

/**
* Takes as input the image with the various plates and the vectors with the boundingboxes. Merges the various bounding boxes in the image
* extrapolates each individual dish and returns a vector containing the boundingboxes for each dish found.
* @param img The input image to apply the boundingboxes to
* @param circlesVector Vector containing the Circular boundingBoxes
* @param rectanglesVector Vector containing the rectangular boundingBoxes
* @param numOfBoxesFirstImg Number of Bounding Boxes Found in the first image
* @return Returns a vector containing the contours of each dish present in the input image
*/
std::vector<std::vector<cv::Point>> subjectIsolator(cv::Mat img, std::vector<cv::Vec3f>& circlesVector, std::vector<cv::Rect>& rectanglesVector, int& numOfBoxesFirstImg);
/**
* Updates the count of the number of bounding boxes and sets it to the number of bounding boxes
* present in the `BBoxVector` vector only if `numOfBoxesFirstImg` is less than 1.
* @param BBoxVector The vector containing the bounding boxes to count.
* @param numOfBoxesFirstImg The number of bounding boxes to set as the initial value.
*/
void rectangleCounter(std::vector<cv::Rect>& BBoxVector, int& numOfBoxesFirstImg);
/**
* Removes a bounding box from the `BBoxVector` vector if the current number of bounding boxes
* is greater than `numOfBoxesFirstImg`. The rectangle with the smallest area among those present is removed.
* @param BBoxVector The vector containing the bounding boxes from which to remove an element.
* @param numOfBoxesFirstImg The maximum number of bounding boxes to keep in the vector.
* @return A new vector containing the bounding boxes after removal (if applicable).
*/
std::vector<cv::Rect> removeUsefullBBox(std::vector<cv::Rect> BBoxVector, int numOfBoxesFirstImg);
/**
* Tests whether the `rect1` rectangle is completely contained in the `rect2` rectangle.
* @param rect1 The first rectangle to test.
* @param rect2 The second rectangle that can contain the first.
* @return True if `rect1` is completely contained in `rect2`, False otherwise.
*/
bool isCompletelyContained(const cv::Rect& rect1, const cv::Rect& rect2);
/**
* Removes rectangles completely contained within other rectangles within the `boundingRectangles` vector.
* @param boundingRectangles The vector of rectangles to remove the contained ones from.
*/
void removeContainedRectangles(std::vector<cv::Rect>& boundingRectangles);
/**
* Sets pixels outside the specified rectangle in the image to zero.
* @param img The input image.
* @param rect The rectangle within which pixels will be kept.
* @return An image with the pixels outside the rectangle set to zero.
*/
cv::Mat setPixelsOutsideRectangle(cv::Mat img, const cv::Rect& rect);
/**
* Apply K-Means algorithm to cluster pixels in the image.
* @param img The input image to cluster.
* @return An image with pixels grouped into colored clusters.
*/
cv::Mat kmeansClustering(cv::Mat img);
/**
* Sets pixels outside the specified boundaries in the image to zero.
* @param image The input image.
* @param contours A vector of point vectors representing contours.
* @return An image with pixels outside the boundaries set to zero (black).
*/
cv::Mat setPixelsOutsideContours(cv::Mat image, const std::vector<std::vector<cv::Point>>& contours);
/**
* Tests whether a rectangle is entirely contained within at least one of the rectangles
* present in the `rectVector` vector.
* @param rect The rectangle to test.
* @param rectVector The vector of rectangles to test against.
* @return True if the rectangle is entirely contained in at least one of the rectangles
* of the vector, otherwise False
*/
bool isRectangleInsideVector(const cv::Rect& rect, const std::vector<cv::Rect>& rectVector);
/**
* Check if the dish has one or more dishes (for example if there are side dishes)
* If found, divides them into a vector of contours.
* @param image Image of the dish to check
* @return Returns a vector of side dishes of the various dishes if they are present
* more than one dish in the image otherwise an empty vector will be returned
*/
std::vector<std::vector<cv::Point>> sideDishSeparator(cv::Mat image);
/**
* Creates a binary mask based on the non-black (not completely black) pixels present
* in the `secondImg` input image.
* @param image The target image on which the mask will be applied.
* @param secondImg The source image from which to create the mask.
* @return A grayscale binary mask where non-black pixels in `secondImg`
* are set to 255 (white) and black pixels to 0 (black).
*/
cv::Mat maskBuilder(cv::Mat image, cv::Mat secondImg);
/**
* Find the largest connected component in the input binary image `mask` after erosion
* optional with a specific structural element.
* @param mask The input binary image containing the connected components.
* @param eroSize The size of the structural element for erosion (0 for no erosion).
* @return A binary image containing only the largest connected component.
*/
cv::Mat findLargestConnectedComponent(const cv::Mat& mask, int eroSize);

#endif