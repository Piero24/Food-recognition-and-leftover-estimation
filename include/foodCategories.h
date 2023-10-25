#ifndef FOODCATEGORIES_H
#define FOODCATEGORIES_H

#include <iostream>
#include <vector>
#include <tuple>
#include <string>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"


struct FoodResult {
    std::string foodName;
    int foodNumber;
    cv::Scalar foodColor;
};
/**
* Identify the type of food in the image
* @param img The input image to process.
* @param clusteredImg Resulting image of clustering.
* @param labels Labels returned from the clustering.
* @param centers Centroids returned from the clustering.
* @return The structure with the food information.
*/
FoodResult foodSelection(cv::Mat img, cv::Mat clusteredImg, cv::Mat labels, cv::Mat centers);
/**
* Performs color pixel counting on a clustered image and returns information about each cluster.
* @param clusteredImg Resulting image of clustering.
* @param labels Labels returned from the clustering.
* @param centers Centroids returned from the clustering.
* @return A vector of tuples containing color information, total colored pixel count, and pixel count for each cluster.
*/
std::vector<std::tuple<cv::Vec3f, double, double>> clusterColorPixelCounter(cv::Mat clusteredImg, cv::Mat labels, cv::Mat centers);
/**
* Calculate the percentage of colored pixels for each color cluster.
* @param pixelColorVector A vector of tuples containing color information, total pixel count, and colored pixel count for each cluster.
* @return A vector of tuples containing color information and the rounded percentage of colored pixels for each cluster.
*/
std::vector<std::tuple<cv::Vec3f, double>> pixelPercentage(std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector);
/**
* Check if the input image corresponds to pasta with pesto based on color analysis.
* @param img Input image to analyze.
* @param clusteredImg Clustered image used for color analysis.
* @param pixelColorVector A vector of tuples containing color information, total pixel count, and colored pixel count for each cluster.
* @return True if the image represents pasta with pesto, otherwise false.
*/
bool pastaWithPesto(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector);
/**
* Check if the input image corresponds to pasta with tomato sauce based on color analysis.
* @param img Input image to analyze.
* @param clusteredImg Clustered image used for color analysis.
* @param pixelColorVector A vector of tuples containing color information, total pixel count, and colored pixel count for each cluster.
* @return True if the image represents pasta with tomato sauce, otherwise false.
*/
bool pastaWithTomatoSauce(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector);
/**
* Check if the input image corresponds to pasta with meat sauce based on color analysis.
* @param img Input image to analyze.
* @param clusteredImg Clustered image used for color analysis.
* @param pixelColorVector A vector of tuples containing color information, total pixel count, and colored pixel count for each cluster.
* @return True if the image represents pasta with meat sauce, otherwise false.
*/
bool pastaWithMeatSauce(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector);
/**
* Check if the input image corresponds to pasta with clams mussels based on color analysis.
* @param img Input image to analyze.
* @param clusteredImg Clustered image used for color analysis.
* @param pixelColorVector A vector of tuples containing color information, total pixel count, and colored pixel count for each cluster.
* @return True if the image represents pasta with clams mussels, otherwise false.
*/
bool pastaWithClamsMussels(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector);
/**
* Check if the input image corresponds to pilaw rice based on color analysis.
* @param img Input image to analyze.
* @param clusteredImg Clustered image used for color analysis.
* @param pixelColorVector A vector of tuples containing color information, total pixel count, and colored pixel count for each cluster.
* @return True if the image represents pilaw rice, otherwise false.
*/
bool pilawRice(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector);
/**
* Check if the input image corresponds to grilled pork cutlet based on color analysis.
* @param img Input image to analyze.
* @param clusteredImg Clustered image used for color analysis.
* @param pixelColorVector A vector of tuples containing color information, total pixel count, and colored pixel count for each cluster.
* @return True if the image represents grilled pork cutlet, otherwise false.
*/
bool grilledPorkCutlet(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector);
/**
* Check if the input image corresponds to fish cutlet based on color analysis.
* @param img Input image to analyze.
* @param clusteredImg Clustered image used for color analysis.
* @param pixelColorVector A vector of tuples containing color information, total pixel count, and colored pixel count for each cluster.
* @return True if the image represents fish cutlet, otherwise false.
*/
bool fishCutlet(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector);
/**
* Check if the input image corresponds to rabbit based on color analysis.
* @param img Input image to analyze.
* @param clusteredImg Clustered image used for color analysis.
* @param pixelColorVector A vector of tuples containing color information, total pixel count, and colored pixel count for each cluster.
* @return True if the image represents rabbit, otherwise false.
*/
bool rabbit(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector);
/**
* Check if the input image corresponds to seafood salad based on color analysis.
* @param img Input image to analyze.
* @param clusteredImg Clustered image used for color analysis.
* @param pixelColorVector A vector of tuples containing color information, total pixel count, and colored pixel count for each cluster.
* @return True if the image represents seafood salad, otherwise false.
*/
bool seafoodSalad(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector);
/**
* Check if the input image corresponds to beans based on color analysis.
* @param img Input image to analyze.
* @param clusteredImg Clustered image used for color analysis.
* @param pixelColorVector A vector of tuples containing color information, total pixel count, and colored pixel count for each cluster.
* @return True if the image represents beans, otherwise false.
*/
bool beans(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector);
/**
* Check if the input image corresponds to basil potatoes based on color analysis.
* @param img Input image to analyze.
* @param clusteredImg Clustered image used for color analysis.
* @param pixelColorVector A vector of tuples containing color information, total pixel count, and colored pixel count for each cluster.
* @return True if the image represents basil potatoes, otherwise false.
*/
bool basilPotatoes(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector);
/**
* Check if the input image corresponds to salad based on color analysis.
* @param img Input image to analyze.
* @param clusteredImg Clustered image used for color analysis.
* @param pixelColorVector A vector of tuples containing color information, total pixel count, and colored pixel count for each cluster.
* @return True if the image represents salad, otherwise false.
*/
bool salad(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector);
/**
* Check if the input image corresponds to bread based on color analysis.
* @param img Input image to analyze.
* @param clusteredImg Clustered image used for color analysis.
* @param pixelColorVector A vector of tuples containing color information, total pixel count, and colored pixel count for each cluster.
* @return True if the image represents bread, otherwise false.
*/
bool bread(cv::Mat img, cv::Mat clusteredImg, std::vector<std::tuple<cv::Vec3f, double, double>> pixelColorVector);

#endif