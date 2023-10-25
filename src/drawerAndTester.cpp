#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include <boost/filesystem.hpp>

#include "../include/classificator.h"


cv::Mat boundingBoxTester(cv::Mat img, std::vector<cv::Vec3f> circlesVector, std::vector<cv::Rect> rectanglesVector) {

    cv::Mat clonedImg = img.clone();

    for (const auto& circle : circlesVector) {
        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        cv::circle(clonedImg, center,  cvRound(circle[2]), cv::Scalar(0, 255, 255), 10);
    }

    for (const auto& rect : rectanglesVector) {
        rectangle(clonedImg, rect, cv::Scalar(0, 255, 0), 10);
    }
    return clonedImg;
}


cv::Mat contourAndBBoxDraw(cv::Mat img, std::vector<Food>& VecFoodDetected, bool drawContour, bool drawBoundingBox) {

    cv::Mat cloned = img.clone();

    if (!VecFoodDetected.empty()) {
        for (const auto& food : VecFoodDetected) {

            // Remove the "None" if for debugging
            if (food.name != "None") {
                if (drawContour) {
                    cv::Scalar color(0, 255, 0);
                    cv::drawContours(cloned, food.contour, -1, color, 5);
                }

                if (drawBoundingBox) {
                    cv::Rect boundingRect = cv::boundingRect(food.contour);
                    cv::rectangle(cloned, boundingRect.tl(), boundingRect.br(), food.color, 12);

                    std::string label = food.name;
                    cv::putText(cloned, label, cv::Point(boundingRect.tl().x, boundingRect.tl().y - 20), cv::FONT_HERSHEY_SIMPLEX, 1.3, food.color, 4);
                }
            }
        }
    }
    return cloned;
}


void overlayMasks(const std::string& inputDir, const std::string& outputDir) {
    
    // Check if the output directory exists, if not, create it
    if (!boost::filesystem::exists(outputDir)) {
        boost::filesystem::create_directory(outputDir);
    }

    for (int trayNum = 1; trayNum <= 8; ++trayNum) {
        std::string trayDir = inputDir + "/tray" + std::to_string(trayNum);
        std::string outputTrayDir = outputDir + "/tray" + std::to_string(trayNum);

        if (!boost::filesystem::exists(outputTrayDir)) {
            boost::filesystem::create_directory(outputTrayDir);
        }

        // Load food image and its mask
        std::string foodImagePath = trayDir + "/food_image.jpg";
        std::string foodImageMaskPath = trayDir + "/masks/food_image_mask.png";

        cv::Mat foodImage = cv::imread(foodImagePath, cv::IMREAD_COLOR);
        cv::Mat foodImageMask = cv::imread(foodImageMaskPath, cv::IMREAD_GRAYSCALE);

        if (foodImage.empty() || foodImageMask.empty()) {
            std::cout << "Error loading images in tray" << trayNum << std::endl;
            continue;
        }

        // Overlay mask on the food image
        cv::Mat overlayedImage;
        foodImage.copyTo(overlayedImage, foodImageMask);

        // Save the output image
        std::string outputPath = outputTrayDir + "/food_with_mask.png";
        cv::imwrite(outputPath, overlayedImage);

        // Load leftover images and their masks
        for (int leftoverNum = 1; leftoverNum <= 3; ++leftoverNum) {
            std::string leftoverImagePath = trayDir + "/leftover" + std::to_string(leftoverNum) + ".jpg";
            std::string leftoverImageMaskPath = trayDir + "/masks/leftover" + std::to_string(leftoverNum) + ".png";

            cv::Mat leftoverImage = cv::imread(leftoverImagePath, cv::IMREAD_COLOR);
            cv::Mat leftoverImageMask = cv::imread(leftoverImageMaskPath, cv::IMREAD_GRAYSCALE);

            if (leftoverImage.empty() || leftoverImageMask.empty()) {
                std::cout << "Error loading images in tray" << trayNum << std::endl;
                continue;
            }

            // Overlay mask on the leftover image
            cv::Mat leftoverOverlayedImage;
            leftoverImage.copyTo(leftoverOverlayedImage, leftoverImageMask);

            // Save the output leftover image
            std::string leftoverOutputPath = outputTrayDir + "/leftover" + std::to_string(leftoverNum) + "_with_mask.jpg";
            cv::imwrite(leftoverOutputPath, leftoverOverlayedImage);
        }
    }
}