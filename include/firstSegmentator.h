#ifndef FIRSTSEGMENTATOR_H
#define FIRSTSEGMENTATOR_H

#include <iostream>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"


struct ClusterInfo {
    cv::Vec3f cluster;
    int clusterSize;
};



std::vector<cv::Vec3f> firstSegmentationFunc(cv::Mat img);
bool compareCircles(const cv::Vec3f& circ1, const cv::Vec3f& circ2);
bool isInnerCircle(const cv::Vec3f& innerCircle, const cv::Vec3f& externalCircle, std::vector<cv::Vec3f>& clustersExcluded);
float distance(cv::Vec3f a, cv::Vec3f b);
//bool compareClusterSize(const ClusterInfo& c1, const ClusterInfo& c2);
std::vector<cv::Vec3f> kmeansCircles(std::vector<cv::Vec3f>& circles, float threshold, int thresholdForCluster, int numberToReturn, std::vector<cv::Vec3f>* biggerCircles = nullptr);

#endif


