#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/imagePreprocessing.h"


cv::Mat segmentationPreprocessing(cv::Mat img) {

    std::vector<int> dimensions = imgDimensions(img);

    cv::Mat noBlueImg = removeBlue(img);
    cv::Mat yellowImg = removeYellow(img);

    cv::Mat resultAfterCanny = cannyPreprocessing(yellowImg, 70, 80);

    cv::Mat noWhiteImg = removeColor(resultAfterCanny, cv::Scalar(170, 170, 170), cv::Scalar(255, 255, 255));

    cv::Mat dilatedImg = imageDilate(resultAfterCanny, 7);

    return img;
}


std::vector<int> imgDimensions(cv::Mat img) {
    std::vector<int> dimensions;
    int area = img.cols * img.rows;

    dimensions.push_back(img.cols);   // Image width
    dimensions.push_back(img.rows);   // Image height
    dimensions.push_back(area);       // Image Area

    // std::cout << "Image width : " << dimensions[0] << std::endl;
    // std::cout << "Image height : " << dimensions[1] << std::endl;
    // std::cout << "Image area : " << dimensions[2] << std::endl;

    return dimensions;
}


cv::Mat removeBlue(cv::Mat img) {
    std::cout << "\u26A0  WARNING: the removeBlue() method isn't complete" << std::endl;
    return  img;
}


cv::Mat removeYellow(cv::Mat img) {
    std::cout << "\u26A0  WARNING: the removeYellow() method isn't complete" << std::endl;
    return  img;
}


cv::Mat cannyPreprocessing(cv::Mat img, int alpha1, int alpha2) {

    std::cout << "\u26A0  WARNING: the cannyPreprocessing() method isn't complete" << std::endl;
    return  img;

}


cv::Mat removeColor(cv::Mat img, cv::Scalar lowerBound, cv::Scalar upperBound) {

    std::cout << "\u26A0  WARNING: the removeColor() method isn't complete" << std::endl;
    return  img;
}


cv::Mat imageDilate(cv::Mat img, int kernelSize) {

    std::cout << "\u26A0  WARNING: the imageDilate() method isn't complete" << std::endl;
    return img;
}

