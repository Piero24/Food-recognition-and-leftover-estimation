#include <iostream>
#include <locale>
#include <cmath>
#include <iterator>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/circularBoundingBoxes.h"


std::vector<cv::Vec3f> findCircularBoundingBoxes(cv::Mat img) {
    
    std::vector<cv::Vec3f> finalVector;
    cv::Mat image = img.clone();

    // Thresholds for determining the largest and smallest circles
    float thresholdBiggerCircles = 150.0;
    float thresholdSmallerCircles = 180.0;
    int thresholdForCluster = 10;

    // Pepper the image to find the largest circles
    cv::Mat imgPrepForBC = biggerCirclesPreprocessing(image);

    std::vector<cv::Vec3f> biggerCircles;
    cv::HoughCircles(imgPrepForBC, biggerCircles, cv::HOUGH_GRADIENT, 1, 1, 100, 20, 285, 305);
    std::sort(biggerCircles.begin(), biggerCircles.end(), compareCircles);

    // Clustering larger circles using the k-means algorithm
    std::vector<cv::Vec3f> clusteredBiggerCircles = kmeansCircles(biggerCircles, thresholdBiggerCircles, thresholdForCluster, 2);

    // Pepper the image to find the largest circles
    cv::Mat ImgWithoutBC = smallerCirclesPreprocessing(image, clusteredBiggerCircles);
    
    std::vector<cv::Vec3f> smallerCircles;
    cv::HoughCircles(ImgWithoutBC, smallerCircles, cv::HOUGH_GRADIENT, 1, 1, 100, 30, 200, 230);
    std::sort(smallerCircles.begin(), smallerCircles.end(), compareCircles);

    // Clustering the smaller sized circles using the k-means algorithm, with reference to the larger sized circles
    std::vector<cv::Vec3f> clusteredSmallerCircles = kmeansCircles(smallerCircles, thresholdSmallerCircles, thresholdForCluster, 1, &clusteredBiggerCircles);

    std::copy(clusteredBiggerCircles.begin(), clusteredBiggerCircles.end(), std::back_inserter(finalVector));
    std::copy(clusteredSmallerCircles.begin(), clusteredSmallerCircles.end(), std::back_inserter(finalVector));

    return finalVector;
}


cv::Mat biggerCirclesPreprocessing(cv::Mat img) {

    cv::Mat clonedImg = img.clone();

    cv::Mat grayImage;
    cv::cvtColor(clonedImg, grayImage, cv::COLOR_BGR2GRAY);

    cv::Mat cannyImg;
    cv::Canny(grayImage, cannyImg, 44, 264);

    cv::Mat aftGauss;
    cv::GaussianBlur(cannyImg, aftGauss, cv::Size(5, 5), 0);

    return aftGauss;
}


cv::Mat smallerCirclesPreprocessing(cv::Mat img, std::vector<cv::Vec3f>& clusteredCircles) {

    cv::Mat clonedImg = img.clone();

    // Covers circles already found to avoid conflicts
    for (const auto& circle : clusteredCircles) {
        cv::Point center(circle[0], circle[1]);
        int radius = circle[2];
        cv::circle(clonedImg, center, radius, cv::Scalar(0, 0, 0), -1);
        cv::circle(clonedImg, center, radius, cv::Scalar(0, 0, 255), 10);
    }

    cv::Mat noGrayImg;
    cv::inRange(clonedImg, cv::Scalar(0, 0, 0), cv::Scalar(135, 135, 135), noGrayImg);
    cv::Mat postGray;
    clonedImg.copyTo(postGray, 255 - noGrayImg);

    cv::Mat grayTmpImage;
    cv::cvtColor(postGray, grayTmpImage, cv::COLOR_BGR2GRAY);

    cv::Mat cannyTmpImg;
    cv::Canny(grayTmpImage, cannyTmpImg, 20, 164);

    cv::Mat aftTmpGauss;
    cv::GaussianBlur(cannyTmpImg, aftTmpGauss, cv::Size(5, 5), 0);

    return aftTmpGauss;
}


bool compareCircles(const cv::Vec3f& circ1, const cv::Vec3f& circ2) {

    float radius1 = circ1[2];
    float radius2 = circ2[2];
    double pi = M_PI;

    int area1 = circ1[2] * circ1[2] * pi;
    int area2 = circ2[2] * circ2[2] * pi;

    return area1 > area2;
}


bool isInnerCircle(const cv::Vec3f& innerCircle, const cv::Vec3f& externalCircle, std::vector<cv::Vec3f>& clustersExcluded) {

    cv::Point2f center1(innerCircle[0], innerCircle[1]);
    cv::Point2f center2(externalCircle[0], externalCircle[1]);

    float radius1 = innerCircle[2];
    float radius2 = externalCircle[2];
    float distance = cv::norm(center1 - center2);

    return (distance + radius1) <= radius2+ 150;
}


float distance(cv::Vec3f a, cv::Vec3f b) {
    return std::sqrt(std::pow((a[0] - b[0]), 2) + std::pow((a[1] - b[1]), 2));
}


bool compareClusterSize(const ClusterInfo& c1, const ClusterInfo& c2) {
    return c1.clusterSize > c2.clusterSize;
}



std::vector<cv::Vec3f> kmeansCircles(std::vector<cv::Vec3f>& circles, float threshold, int thresholdForCluster, int numberToReturn, std::vector<cv::Vec3f>* biggerCircles) {
    
    std::vector<cv::Vec3f> result;
    std::vector<ClusterInfo> filteredResult;
    std::vector<cv::Vec3f> sortedClusters;

    if (circles.empty()) {
        return result;
    }

    cv::Vec3f centroid = circles[0];
    result.push_back(centroid);

    // For each pair of circles calculate their distance and check
    // it is less than a given threshold.
    // And calculates the weighted average of the cluster circles.
    // If the distance is greater than the threshold, create a new cluster instead
    for (int i = 1; i < circles.size(); i++) {
        bool foundCluster = false;

        for (int j = 0; j < result.size(); j++) {
            float dist = distance(circles[i], result[j]);
            if (dist <= threshold) {

                foundCluster = true;
                float totalWeight = result[j][2] + circles[i][2];
                result[j][0] = (result[j][0] * result[j][2] + circles[i][0] * circles[i][2]) / totalWeight;
                result[j][1] = (result[j][1] * result[j][2] + circles[i][1] * circles[i][2]) / totalWeight;
                result[j][2] = totalWeight / 2.0;
                break;
            }
        }

        if (!foundCluster) {
            result.push_back(circles[i]);
        }
    }
    
    // Count the number of circles belonging to each cluster
    for (const auto& cluster : result) {
        int clusterSize = 0;

        for (const auto& circle : circles) {
            float dist = distance(circle, cluster);
            if (dist <= threshold) {
                clusterSize++;
            }
        }

        filteredResult.push_back({cluster, clusterSize});
    }

    std::sort(filteredResult.begin(), filteredResult.end(), compareClusterSize);
    
    for (const auto& clusterInfo : filteredResult) {
        if (sortedClusters.size() < numberToReturn) {

            // If null then it is operating on the largest circles
            // Otherwise it is operating on the smaller ones
            if (biggerCircles != nullptr) {
                std::vector<cv::Vec3f> clustersExcluded;
                int flag = 0;

                // Check if the circle is inside one of the circles in the old set
                // (The big ones).
                for (const auto& externalCircle : *biggerCircles) {
                   bool isInner = isInnerCircle(clusterInfo.cluster, externalCircle, clustersExcluded);

                   if (isInner) {
                    flag ++;
                   }
                }
                // If the size is less than the threshold and the circle is not inside one
                 // of the large ones add it to the list of ones to return
                if ((clusterInfo.clusterSize >= thresholdForCluster) && (flag < 1)) {
                        // std::cout << "\U0001F7E1"  << "  Center: (" << clusterInfo.cluster[0] << ", " << clusterInfo.cluster[1] << "), 
                        // Radius: " << clusterInfo.cluster[2] << " Cluster Size: " << clusterInfo.clusterSize << std::endl;
                        sortedClusters.push_back(clusterInfo.cluster);
                    }

            } else {
                // If the size is less than the threshold add it to the list of those to return
                if (clusterInfo.clusterSize >= thresholdForCluster) {
                    // std::cout << "\U0001F7E1"  << "  Center: (" << clusterInfo.cluster[0] << ", " << clusterInfo.cluster[1] << "), 
                    // Radius: " << clusterInfo.cluster[2] << " Cluster Size: " << clusterInfo.clusterSize << std::endl;
                    sortedClusters.push_back(clusterInfo.cluster);
                } else {
                    break;
                } 
            }
        } else {
            break;
        }
    }
    return sortedClusters;
} 