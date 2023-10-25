#ifndef CLASSIFICATOR_H
#define CLASSIFICATOR_H

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

typedef std::tuple<std::string, cv::Mat> StringMatTuple;

/**
* Represents a food object with a name, a side dish and matches.
*/
class Food {
public:
    std::string name;
    int foodNumber;
    std::vector<cv::Point> contour;
    std::vector<cv::DMatch> matches;
    cv::Scalar color;


    // Class constructor
    Food(const std::string& str, const int& num, const std::vector<cv::Point>& points, const cv::Scalar& clrs, const std::vector<cv::DMatch>& dmatches = std::vector<cv::DMatch>())
        : name(str), foodNumber(num), contour(points), matches(dmatches), color(clrs) {
    }
};
/**
* Classifies food objects in the image based on a chosen detection method.
* @param img The input image containing the food objects.
* @param foodContours The outlines of the food objects in the image.
* @param type The type of detection method to use (1 for Sift, 2 for heuristic algorithm).
* @param pathPatch The path for loading reference images (used only with the Sift method).
* @param VecFoodsFirstImage A vector containing tuples (name, array) of reference images (used only with the Sift method).
* @return A vector of classified food objects in the image.
*/
std::vector<Food> foodClassificator(cv::Mat img, std::vector<std::vector<cv::Point>>& foodContours, int type, std::string pathPatch, std::vector<StringMatTuple>& VecFoodsFirstImage);
/**
* Loads a set of images from a specific directory and returns them as a vector of tuples (name, array).
* @param pathPatch The path to the directory containing the images to upload.
* @return An array of tuples containing the name and array of images loaded from the directory.
*/
std::vector<StringMatTuple> patchLoader(std::string pathPatch);
/**
* Perform food object detection and matching within an image using SIFT.
* @param img The input image to perform detection on.
* @param patchVec A vector of tuples containing the name and patch array of known food objects.
* @param foodContours A vector of point vectors representing the contours of food objects in the image.
* @return An array of Food objects containing information about detected food objects.
*/
std::vector<Food> imgMatching(cv::Mat img, std::vector<StringMatTuple>& patchVec, std::vector<std::vector<cv::Point>>& foodContours);
/**
* Preprocess the food in the contour and call the function to classify the type of food
* @param img The input image to perform detection on.
* @param foodContours A vector of point vectors representing the contours of food objects in the image.
* @return A vector containing the foods present in the image.
*/
std::vector<Food> foodIdentifier(cv::Mat img, std::vector<std::vector<cv::Point>>& foodContours);
/**
* Apply preprocessing to the image
* @param img The input image to perform detection on.
* @param mask Region of interest mask for the image.
* @return The processed image.
*/
cv::Mat imgClassificationPreprocessing(cv::Mat img, cv::Mat mask);

#endif