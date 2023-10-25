#ifndef CIRCULARBOUNDINGBOXES_H
#define CIRCULARBOUNDINGBOXES_H

#include <iostream>
#include <locale>
#include <cmath>
#include <iterator>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

/**
* Structure to represent the information of a cluster resulting from the algorithm
* k-means clustering.
*/
struct ClusterInfo {
    cv::Vec3f cluster;
    int clusterSize;
};
/**
* Find circular bounding boxes in the image using the Hough algorithm
* Circle and k-means clustering.
* @param img The input image to search for bounding boxes.
* @return A vector containing the found circular bounding boxes.
*/
std::vector<cv::Vec3f> findCircularBoundingBoxes(cv::Mat img);
/**
* Preprocesses the image to detect larger circles.
* @param img The input image to process.
* @return The image preprocessed with grayscale conversion operations,
* edge detection and smoothing.
*/
cv::Mat biggerCirclesPreprocessing(cv::Mat img);
/**
* Preprocesses the image to detect smaller circles,
* taking into account the larger circles previously identified.
* @param img The input image to process.
* @param clusteredCircles A vector containing the information of the largest circles.
* @return The image preprocessed with masking operations,
* grayscale conversion, edge detection and smoothing.
*/
cv::Mat smallerCirclesPreprocessing(cv::Mat img, std::vector<cv::Vec3f>& clusteredCircles);
/**
* Compare two circles based on area.
* @param circ1 The first circle to compare.
* @param circ2 The second circle to compare.
* @return True if the area of the first circle is greater than the area of the second circle, otherwise false.
*/
bool compareCircles(const cv::Vec3f& circ1, const cv::Vec3f& circ2);
/**
* Tests whether an inner circle is contained completely within an outer circle, excluding any specified circles.
* @param innerCircle The inner circle to check.
* @param externalCircle The external reference circle.
* @param clustersExcluded A vector containing circles to exclude from testing.
* @return True if the inner circle is completely contained in the outer circle, false otherwise.
*/
bool isInnerCircle(const cv::Vec3f& innerCircle, const cv::Vec3f& externalCircle, std::vector<cv::Vec3f>& clustersExcluded);
/**
* Calculate the Euclidean distance between two points represented as three-dimensional vectors.
* @param a The first point.
* @param b The second point.
* @return The Euclidean distance between the two points.
*/
float distance(cv::Vec3f a, cv::Vec3f b);
/**
* Compares two ClusterInfo objects based on cluster size, returning true if the size of the first cluster is larger than the size of the second cluster.
* @param c1 The first ClusterInfo object to compare.
* @param c2 The second ClusterInfo object to compare.
* @return True if the size of the first cluster is larger than the size of the second cluster, false otherwise.
*/
bool compareClusterSize(const ClusterInfo& c1, const ClusterInfo& c2);
/**
* Runs k-means algorithm to group circles into clusters based on their distance.
* @param circles The circles to group.
* @param threshold The distance threshold for considering two circles as part of the same cluster.
* @param thresholdForCluster The minimum size threshold to consider a cluster valid.
* @param numberToReturn The maximum number of clusters to return.
* @param biggerCircles A pointer to a vector of bigger circles (optional).
* @return The clustered circles.
*/
std::vector<cv::Vec3f> kmeansCircles(std::vector<cv::Vec3f>& circles, float threshold, int thresholdForCluster, int numberToReturn, std::vector<cv::Vec3f>* biggerCircles = nullptr);


#endif


