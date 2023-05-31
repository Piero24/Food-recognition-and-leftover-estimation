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
    cv::Mat lighterImager = lightIncreaser(img);
    cv::Mat resultAfterCanny = cannyPreprocessing(img, 70, 80);

    // Remove white from the image
    cv::Mat noWhiteImg = removeColor(resultAfterCanny, cv::Scalar(170, 170, 170), cv::Scalar(255, 255, 255));
    // Remove gray from the image
    cv::Mat noGrayImg = removeColor(noWhiteImg, cv::Scalar(50, 50, 50), cv::Scalar(190, 190, 190));

    cv::Mat dilatedImg = imageDilate(noGrayImg, 7);

    // Remove white from the image
    noWhiteImg.setTo(cv::Scalar(0, 0, 0));
    noWhiteImg = removeColor(dilatedImg, cv::Scalar(170, 170, 170), cv::Scalar(255, 255, 255));
    // Remove gray from the image
    noGrayImg.setTo(cv::Scalar(0, 0, 0));
    noGrayImg = removeColor(noWhiteImg, cv::Scalar(50, 50, 50), cv::Scalar(190, 190, 190));

    // Remove dark gray from the image
    cv:: Mat noDarkGrayImg = removeColor(noGrayImg, cv::Scalar(0, 0, 0), cv::Scalar(60, 60, 60));

    cv::Mat finalResult = imageDilate(noDarkGrayImg, 5);

    return finalResult;

}


std::vector<int> imgDimensions(cv::Mat img) {
    std::vector<int> dimensions;
    int area = img.cols * img.rows;

    dimensions.push_back(img.cols);   // Image width
    dimensions.push_back(img.rows);   // Image height
    dimensions.push_back(area);       // Image Area

    //std::cout << "Image width : " << dimensions[0] << std::endl;
    //std::cout << "Image height : " << dimensions[1] << std::endl;
    //std::cout << "Image area : " << dimensions[2] << std::endl;

    return dimensions;
}


cv::Mat removeBlue(cv::Mat img) {

    // TODO: BISOGNA LAVORE SUL BLUE YOUGURTH E PANTALONI TRAY5
    cv::Scalar lowerBoundBlue(30, 20, 5);
    cv::Scalar upperBoundBlue(255, 170, 120);

    cv::Mat maskBlue;
    cv::inRange(img, lowerBoundBlue, upperBoundBlue, maskBlue);

    cv::Mat postMaskBlue;
    img.copyTo(postMaskBlue, 255 - maskBlue);

    std::cout << "\u26A0  WARNING: the removeBlue() method isn't complete" << std::endl;

    return  img;

}


cv::Mat lightIncreaser(cv::Mat img) {

    cv::Mat lighterImage;
    double alpha = 1.0;
    int beta = 80;
    img.convertTo(lighterImage, -1, alpha, beta);

    return  lighterImage;

}


cv::Mat cannyPreprocessing(cv::Mat img, int alpha1, int alpha2) {

    cv::Mat cannyImg;
    cv::Canny(img, cannyImg, alpha1, alpha2);

    // Only keep the selected item from Canny
    cv::Mat resultAfterCanny;
    cv::bitwise_and(img, img, resultAfterCanny, cannyImg);

    return resultAfterCanny;

}


cv::Mat removeColor(cv::Mat img, cv::Scalar lowerBound, cv::Scalar upperBound) {

    cv::Mat mask;
    cv::inRange(img, lowerBound, upperBound, mask);

    cv::Mat postMask;
    img.copyTo(postMask, 255 - mask);

    return  postMask;

}

cv::Mat imageDilate(cv::Mat img, int kernelSize) {

    cv::Mat dilatedImg;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernelSize, kernelSize));
    cv::dilate(img, dilatedImg, kernel);

    return dilatedImg;

}

