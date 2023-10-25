#include <iostream>
#include <vector>
#include <tuple>
#include <string>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include <opencv2/features2d.hpp>

#include "../include/classificator.h"
#include "../include/detector.h"
#include "../include/foodCategories.h"


std::vector<Food> foodClassificator(cv::Mat img, std::vector<std::vector<cv::Point>>& foodContours, int type, std::string pathPatch, std::vector<StringMatTuple>& VecFoodsFirstImage)  {

    // Two Different method that can be chosen by the user.
    // 1) By Sift (NOT)
    // 2) Euristic algorithm based on some charatteristic

    std::vector<Food> VecFoodDetected;

    if (type == 1) {
        if (VecFoodsFirstImage.empty()) {
            std::vector<StringMatTuple> patchVec = patchLoader(pathPatch);
            VecFoodDetected = imgMatching(img, patchVec, foodContours);

            for (const auto& food : VecFoodDetected) {
                for (const auto& patch : patchVec) {

                    std::string foodName = std::get<0>(patch);
                    int result = foodName.compare(food.name);

                    if (result == 0) {
                        VecFoodsFirstImage.push_back(patch);
                    }
                }
            }

        }
        else {
            VecFoodDetected = imgMatching(img, VecFoodsFirstImage, foodContours);
        }

    } else {
        VecFoodDetected = foodIdentifier(img, foodContours);
    }

    return VecFoodDetected;
}


std::vector<StringMatTuple> patchLoader(std::string pathPatch) {

    std::vector<StringMatTuple> imageVec;
    std::vector<cv::String> fileInFolderVec;

    cv::glob(pathPatch, fileInFolderVec, false);
    
    size_t count = fileInFolderVec.size();
    for (size_t i = 0; i < count; i++) {
        // Verify if it is an images
        if (fileInFolderVec[i].substr(fileInFolderVec[i].size() - 4) == ".jpg" ||
            fileInFolderVec[i].substr(fileInFolderVec[i].size() - 4) == ".JPG" ||
            fileInFolderVec[i].substr(fileInFolderVec[i].size() - 4) == ".png" ||
            fileInFolderVec[i].substr(fileInFolderVec[i].size() - 4) == ".bmp") { 

            cv::Mat image = cv::imread(fileInFolderVec[i]); 

            if (!image.empty()) {

                size_t lastSlashPos = fileInFolderVec[i].find_last_of('/');
                std::string imgName = fileInFolderVec[i].substr(lastSlashPos + 1);
                imgName = imgName.substr(0, imgName.length() - 4);

                StringMatTuple tupleStrMat = std::make_tuple(imgName, image);
                imageVec.push_back(tupleStrMat);
            } 
        } 
    }
    
    return imageVec;
}


std::vector<Food> imgMatching(cv::Mat img, std::vector<StringMatTuple>& patchVec, std::vector<std::vector<cv::Point>>& foodContours) {
    
    std::cout << "\U0001F7E3  WARNING: imgMatching() method isn't complete" << std::endl;

    std::vector<Food> foodFounded;

    cv::Mat clonedImg = img.clone();

    for (const auto& cnt : foodContours) {

            std::vector<cv::Point> contour = cnt;

            cv::Mat mask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);

            std::vector<cv::DMatch> BestGoodMatches;
            std::string nameBestFoodDetected = "";
            // std::vector<cv::Point> BestFoodContour;

            std::vector<std::vector<cv::Point>> contours = {contour};
            cv::fillPoly(mask, contours, cv::Scalar(255));

            cv::Mat tempMask = mask.clone();
            cv::drawContours(tempMask, contours, -1, cv::Scalar(0), 10);

            int nonZeroPixels = cv::countNonZero(tempMask);

            if (nonZeroPixels > 10000) {
            mask = tempMask.clone();
            }

            cv::Mat foodToIdentify;
            clonedImg.copyTo(foodToIdentify, mask);


            //cv::imshow("Result", foodToIdentify);
            //cv::waitKey(0);

            for (const auto& patchTuple : patchVec) {

                std::string foodName = std::get<0>(patchTuple);
                cv::Mat foodPatch = std::get<1>(patchTuple);

                // Initiate SIFT detector 
                cv::Ptr<cv::SIFT> sift = cv::SIFT::create(); 
            
                // Find the keypoints and descriptors with SIFT 
                std::vector<cv::KeyPoint> keypoints1, keypoints2; 
                cv::Mat descriptor1, descriptor2; 
                sift -> detectAndCompute(foodToIdentify, cv::noArray(), keypoints1, descriptor1); 
                sift -> detectAndCompute(foodPatch, cv::noArray(), keypoints2, descriptor2);

                // Initialize the FlannBasedMatcher
                cv::FlannBasedMatcher matcher;

                // Match image and patch descriptors
                std::vector<cv::DMatch> matches;
                matcher.match(descriptor2, descriptor1, matches);

                // Calculate the maximum and minimum distance between matches
                double max_dist = 0;
                double min_dist = std::numeric_limits<double>::max();

                for (int i = 0; i < matches.size(); i++) {
                    double dist = matches[i].distance;
                    if (dist < min_dist) min_dist = dist;
                    if (dist > max_dist) max_dist = dist;
                }

                // Filter matches using a threshold (you can choose the threshold according to your needs)
                //double threshold = 0.7 * max_dist;
                double threshold = 0.45 * max_dist;

                std::vector<cv::DMatch> goodMatches;
                for (int i = 0; i < matches.size(); i++) {
                    if (matches[i].distance < threshold) {
                        goodMatches.push_back(matches[i]);
                    }
                }

                int goodMatchesDistance = 0;
                int BestGoodMatchesDistance = 0;

                for (const cv::DMatch& match : goodMatches) {
                    goodMatchesDistance += match.distance;
                }

                for (const cv::DMatch& bestMatch : BestGoodMatches) {
                    BestGoodMatchesDistance += bestMatch.distance;
                }
                // DEBUGGING
                // std::cout << "PATCH DISTANCE ( " << foodName << " ) ACTUAL: " << goodMatchesDistance << std::endl;
                // std::cout << "BEST PATCH DISTANCE ( " << nameBestFoodDetected << " ): " << BestGoodMatchesDistance  << "\n" << std::endl;

                if ((goodMatchesDistance < BestGoodMatchesDistance) || (BestGoodMatches.empty())) {
                    BestGoodMatches = goodMatches;
                    nameBestFoodDetected = foodName;
                }
            }

            Food detectedFood(nameBestFoodDetected, 0, cnt, cv::Scalar(0, 0, 255), BestGoodMatches);
            foodFounded.push_back(detectedFood);

        }

    return foodFounded;
}


std::vector<Food> foodIdentifier(cv::Mat img, std::vector<std::vector<cv::Point>>& foodContours) {

    std::vector<Food> foodFounded;
    cv::Mat clonedImg = img.clone();

    for (size_t i = 0; i < foodContours.size(); i++) {


        cv::Mat mask = cv::Mat::zeros(clonedImg.size(), CV_8UC1);
        cv::drawContours(mask, foodContours, static_cast<int>(i), cv::Scalar(255), cv::FILLED);

        cv::Mat contoursImg;
        clonedImg.copyTo(contoursImg, mask);

        double area = cv::contourArea(foodContours[i]);

        cv::Mat imgPostErosion = imgClassificationPreprocessing(clonedImg, mask);



        cv::Mat gaussianImg;
        cv::GaussianBlur(imgPostErosion, gaussianImg, cv::Size(81, 81), 0);

        // Reshape the image into a 1D array of points (each row represents a pixel)
        cv::Mat data = gaussianImg.reshape(1, gaussianImg.cols * gaussianImg.rows);
        data.convertTo(data, CV_32F);

        // Number of clusters desired
        int K = 2;
        cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 100, 0.1);

        cv::Mat labels, centers;
        cv::kmeans(data, K, labels, criteria, 10, cv::KMEANS_RANDOM_CENTERS, centers);

        // Replace the pixel colors with the centroid colors of the corresponding cluster
        cv::Mat clusteredImage(gaussianImg.size(), gaussianImg.type());

        for (int i = 0; i < gaussianImg.rows; ++i) {
            for (int j = 0; j < gaussianImg.cols; ++j) {
                int label = labels.at<int>(i * gaussianImg.cols + j);
                clusteredImage.at<cv::Vec3b>(i, j) = centers.at<cv::Vec3f>(label, 0);
            }
        }

        FoodResult strOuput = foodSelection(contoursImg, clusteredImage, labels, centers);
        Food detectedFood(strOuput.foodName, strOuput.foodNumber, foodContours[i], strOuput.foodColor);
        foodFounded.push_back(detectedFood);
    }
    
    return foodFounded;
}


cv::Mat imgClassificationPreprocessing(cv::Mat img, cv::Mat mask) {
    
    cv::Mat clonedImg = img.clone();

    cv::Mat contoursImg;
    clonedImg.copyTo(contoursImg, mask);

    cv::Scalar targetColor_1(157, 158, 156);
    cv::Scalar targetColor_2(205, 199, 194);
    cv::Scalar targetColor_3(98, 98, 113);
    
    int colorRange = 25;

    for (int y = 0; y < contoursImg.rows; ++y) {
        for (int x = 0; x < contoursImg.cols; ++x) {

            cv::Vec3b color = contoursImg.at<cv::Vec3b>(y, x);

            if ((abs(color[0] - targetColor_1[0]) <= colorRange &&
                abs(color[1] - targetColor_1[1]) <= colorRange &&
                abs(color[2] - targetColor_1[2]) <= colorRange) ||

                (abs(color[0] - targetColor_2[0]) <= colorRange &&
                abs(color[1] - targetColor_2[1]) <= colorRange &&
                abs(color[2] - targetColor_2[2]) <= colorRange) ||

                (abs(color[0] - targetColor_3[0]) <= colorRange &&
                abs(color[1] - targetColor_3[1]) <= colorRange &&
                abs(color[2] - targetColor_3[2]) <= colorRange)) {

                contoursImg.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
            }
        }
    }

    cv::Mat newMask = maskBuilder(contoursImg, contoursImg);

    int erosionSize = 1;
    cv::Mat elementEro = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize));

    cv::Mat erodImage;
    cv::erode(newMask, erodImage, elementEro);

    cv::Mat largestComponent = findLargestConnectedComponent(erodImage, 0);

    cv::Mat imgPostErosion;
    clonedImg.copyTo(imgPostErosion, largestComponent);

    return imgPostErosion;
}