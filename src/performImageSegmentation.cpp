// make
// ./Food-Recognition ./Food-recognition-and-leftover-estimation/dataset/tray1/
// ./Food-Recognition ./Food-recognition-and-leftover-estimation/dataset/tray2/

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/performImageSegmentation.h"


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