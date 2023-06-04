#include <iostream>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/rectangularBoundingBoxes.hpp"


std::vector<cv::Rect> findRectangularBoundingBoxes(cv::Mat img, cv::Mat img2) {

    std::vector<cv::Rect> finalVector;
    cv::Mat clonedImg = img2.clone();
    cv::Mat imgOut = img.clone();

    std::vector<cv::Rect> rects = recSegmentation(clonedImg, "f");

    //std::cout << "Total Number of Region Proposals: " << rects.size() << std::endl;
    
    std::sort(rects.begin(), rects.end(), compareRectangles);
    std::vector<cv::Rect> noToBigNoToSmall = pushOut(rects);
    std::vector<cv::Rect> outerRectangles = removeInnerRectangles(noToBigNoToSmall);

    int numShowRects = outerRectangles.size();
    size_t outerRectanglesSize = outerRectangles.size();

    for (size_t j = 0; j < outerRectanglesSize; j++) {
        int areaRect = outerRectangles[j].width * outerRectangles[j].height;

        if (j < numShowRects) {
            cv::Rect roi = outerRectangles[j];
            int nonBlackPixelCount = 0;

            for (int y = roi.y; y < (roi.y + roi.height); y++) {
                for (int x = roi.x; x < (roi.x + roi.width); x++) {
                    cv::Vec3b pixel = clonedImg.at<cv::Vec3b>(y, x);
                    if (pixel != cv::Vec3b(0, 0, 0)) {
                        nonBlackPixelCount++;
                    }
                }
            }

            double density = static_cast<double>(nonBlackPixelCount) / (roi.width * roi.height);

            if (density > 0.22) {
                std::cout << "\U0001F7E2" << "  Rect: " << outerRectangles[j] << " Area: " << roi.width * roi.height << " Density: " << density << " Non Black Pixel: " << nonBlackPixelCount << std::endl;
                finalVector.push_back(outerRectangles[j]);

            } else {
                std::cout << "\U0001F534" << "  Rect: " << outerRectangles[j] << " Area: " << roi.width * roi.height << " Density: " << density << " Non Black Pixel: " << nonBlackPixelCount << std::endl;
            }
            
        }
        else {
            break;
        }
    }

    return finalVector;
}


bool compareRectangles(const cv::Rect& rect1, const cv::Rect& rect2) {
    int area1 = rect1.width * rect1.height;
    int area2 = rect2.width * rect2.height;
    return area1 > area2;
}


bool isOverlap(const cv::Rect& rect1, const cv::Rect& rect2) {
    return (rect1 & rect2).area() > 0;
}


std::vector<cv::Rect> removeInnerRectangles(const std::vector<cv::Rect>& rectangles) {
    std::vector<cv::Rect> result;

    for (const cv::Rect& rect : rectangles) {
        bool isInner = false;

        for (const cv::Rect& existingRect : result) {
            if (isOverlap(rect, existingRect)) {
                isInner = true;
                break;
            }
        }

        if (!isInner) {
            result.push_back(rect);
        }
    }

    return result;
}


std::vector<cv::Rect> pushOut(const std::vector<cv::Rect>& rectangles) {

    std::vector<cv::Rect> result;

    for (const cv::Rect& rect : rectangles) {
        int areaRect = rect.width * rect.width;
        //if ((areaRect > 50800) && (areaRect  < 428800)) {
        if ((areaRect > 20800) && (areaRect < 428800)) {
            result.push_back(rect);
        }
    }
    return result;
}


std::vector<cv::Rect> recSegmentation(cv::Mat img, std::string quality) {

    cv::Ptr<cv::ximgproc::segmentation::SelectiveSearchSegmentation> ss = cv::ximgproc::segmentation::createSelectiveSearchSegmentation();

    cv::Ptr<cv::ximgproc::segmentation::SelectiveSearchSegmentationStrategyColor> colorStrategy = cv::ximgproc::segmentation::createSelectiveSearchSegmentationStrategyColor();
    cv::Ptr<cv::ximgproc::segmentation::SelectiveSearchSegmentationStrategyTexture> textureStrategy = cv::ximgproc::segmentation::createSelectiveSearchSegmentationStrategyTexture();
    cv::Ptr<cv::ximgproc::segmentation::SelectiveSearchSegmentationStrategyMultiple> multipleStrategy = cv::ximgproc::segmentation::createSelectiveSearchSegmentationStrategyMultiple(colorStrategy, textureStrategy);

    ss -> addStrategy(multipleStrategy);
    ss -> setBaseImage(img);

    if (quality == "f") {
        ss -> switchToSelectiveSearchFast();

    } else {
        ss -> switchToSelectiveSearchQuality();
    }

    std::vector<cv::Rect> rects;
    ss -> process(rects);

    return rects;
}
