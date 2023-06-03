#include <iostream>
#include <locale>
#include <cmath>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"


#include "../include/firstSegmentator.h"

// Pallino giallo:
//     Codice Unicode: \U0001F7E1
//     Valore RGB: (255, 255, 0)
//     Valore BGR: (0, 255, 255)


//ceramica 62-71
//vetro 48-54

//Cerchi grandi
int minRadiusH = 285; 
int maxRadiusH = 305; 

// Cerchi piccoli
int minRadius = 200; 
int maxRadius = 230; 

float distance(cv::Vec3f a, cv::Vec3f b);
std::vector<cv::Vec3f> kmeansCircles(std::vector<cv::Vec3f>& circles, float threshold, int thresholdForCluster, int numberToReturn, std::vector<cv::Vec3f>* biggerCircles = nullptr);


cv::Mat firstSegmentationFunc(cv::Mat img) {

    cv::Mat image = img.clone();
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    cv::Mat cannyImg;
    cv::Canny(grayImage, cannyImg, 44, 264);

    cv::Mat aftGauss;
    cv::GaussianBlur(cannyImg, aftGauss, cv::Size(5, 5), 0);

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(aftGauss, circles, cv::HOUGH_GRADIENT, 1, 1, 100, 20, minRadiusH, maxRadiusH);
    std::sort(circles.begin(), circles.end(), compareCircles);

    float threshold = 150.0;  // Soglia per considerare i cerchi come simili
    int thresholdForCluster = 10;

    std::vector<cv::Vec3f> clusteredCircles = kmeansCircles(circles, threshold, thresholdForCluster, 2);

    cv::Mat tmpImg = image.clone();

    for (const auto& circle : clusteredCircles) {
        cv::Point center(circle[0], circle[1]);
        int radius = circle[2];
        cv::circle(tmpImg, center, radius, cv::Scalar(0, 0, 0), -1);  // Disegna un cerchio nero sul cerchio corrispondente
    }
    
    cv::Mat grayTmpImage;
    cv::cvtColor(tmpImg, grayTmpImage, cv::COLOR_BGR2GRAY);

    cv::Mat cannyTmpImg;
    cv::Canny(grayTmpImage, cannyTmpImg, 44, 264);

    cv::Mat aftTmpGauss;
    cv::GaussianBlur(cannyTmpImg, aftTmpGauss, cv::Size(5, 5), 0);

    std::vector<cv::Vec3f> smallerCircles;
    cv::HoughCircles(aftTmpGauss, smallerCircles, cv::HOUGH_GRADIENT, 1, 1, 100, 20, minRadius, maxRadius);
    std::sort(smallerCircles.begin(), smallerCircles.end(), compareCircles);

    std::vector<cv::Vec3f> clusteredSmallerCircles = kmeansCircles(smallerCircles, threshold, thresholdForCluster, 1, &circles);

    cv::Mat resultImage = image.clone();
    for (const auto& circle : smallerCircles) {

        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        int radius = cvRound(circle[2]);
        cv::circle(resultImage, center, radius, cv::Scalar(0, 255, 0), 2);
    }

    for (const auto& circle : clusteredCircles) {

        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        int radius = cvRound(circle[2]);
        cv::circle(resultImage, center, radius, cv::Scalar(0, 0, 255), 10);
    }

    for (const auto& circle : clusteredSmallerCircles) {

        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        int radius = cvRound(circle[2]);
        cv::circle(resultImage, center, radius, cv::Scalar(255, 0, 0), 10);
    }

    std::cout << "\U0001F7E1  WARNING: the thirdSegmentationFunc() method isn't complete" << std::endl;

    return resultImage;

}


bool compareCircles(const cv::Vec3f& circ1, const cv::Vec3f& circ2) {

    float radius1 = circ1[2];
    float radius2 = circ2[2];
    double pi = M_PI;

    int area1 = circ1[2] * circ1[2] * pi;
    int area2 = circ2[2] * circ2[2] * pi;

    return area1 > area2;
}


bool isInnerCircle(const cv::Vec3f& innerCircle, const cv::Vec3f& externalCircle) {
    cv::Point2f center1(innerCircle[0], innerCircle[1]);
    cv::Point2f center2(externalCircle[0], externalCircle[1]);
    float radius1 = innerCircle[2];
    float radius2 = externalCircle[2];

    float distance = cv::norm(center1 - center2);

    return (distance + radius1) <= radius2;
}





float distance(cv::Vec3f a, cv::Vec3f b) {
    return std::sqrt(std::pow((a[0] - b[0]), 2) + std::pow((a[1] - b[1]), 2));
}


struct ClusterInfo {
    cv::Vec3f cluster;
    int clusterSize;
};


bool compareClusterSize(const ClusterInfo& c1, const ClusterInfo& c2) {
    return c1.clusterSize > c2.clusterSize;
}


std::vector<cv::Vec3f> kmeansCircles(std::vector<cv::Vec3f>& circles, float threshold, int thresholdForCluster, int numberToReturn, std::vector<cv::Vec3f>* biggerCircles) {
    std::vector<cv::Vec3f> result;

    if (circles.empty()) {
        return result;
    }

    cv::Vec3f centroid = circles[0];
    result.push_back(centroid);


    for (int i = 1; i < circles.size(); i++) {
        bool foundCluster = false;


        for (int j = 0; j < result.size(); j++) {
            float dist = distance(circles[i], result[j]);
            if (dist <= threshold) {
                foundCluster = true;

                // Calcola la media pesata dei centri
                float totalWeight = result[j][2] + circles[i][2];
                // Media pesata delle coordinate X
                result[j][0] = (result[j][0] * result[j][2] + circles[i][0] * circles[i][2]) / totalWeight;
                // Media pesata delle coordinate Y
                result[j][1] = (result[j][1] * result[j][2] + circles[i][1] * circles[i][2]) / totalWeight;

                // Calcola la media pesata dei raggi
                result[j][2] = totalWeight / 2.0;

                break;
            }
        }

        if (!foundCluster) {
            result.push_back(circles[i]);
        }
    }


    std::vector<ClusterInfo> filteredResult;
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

    std::vector<cv::Vec3f> sortedClusters;
    for (const auto& clusterInfo : filteredResult) {
        //std::cout << "\U0001F7E1"  << "  Center: (" << clusterInfo.cluster[0] << ", " << clusterInfo.cluster[1] << "), Radius: " << clusterInfo.cluster[2] << " Cluster Size: " << clusterInfo.clusterSize << std::endl;

        if (sortedClusters.size() < numberToReturn) {

            if (biggerCircles != nullptr) {
                for (const auto& externalCircle : *biggerCircles) {
                   bool isInner = isInnerCircle(clusterInfo.cluster, externalCircle);

                   std::cout << (clusterInfo.clusterSize >= thresholdForCluster)  << ", " << isInner << std::endl;

                   if ((clusterInfo.clusterSize >= thresholdForCluster) && !isInner) {
                        std::cout << "\U0001F7E1"  << "  Center: (" << clusterInfo.cluster[0] << ", " << clusterInfo.cluster[1] << "), Radius: " << clusterInfo.cluster[2] << " Cluster Size: " << clusterInfo.clusterSize << std::endl;
                        sortedClusters.push_back(clusterInfo.cluster);
                    }
                }
            } else {
                if (clusterInfo.clusterSize >= thresholdForCluster) {
                    std::cout << "\U0001F7E1"  << "  Center: (" << clusterInfo.cluster[0] << ", " << clusterInfo.cluster[1] << "), Radius: " << clusterInfo.cluster[2] << " Cluster Size: " << clusterInfo.clusterSize << std::endl;
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