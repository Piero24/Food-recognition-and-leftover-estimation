#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/imagePreprocessing.h"


cv::Mat segmentationPreprocessing(cv::Mat img) {

    std::vector<int> dimensions = imgDimensions(img);
    
    cv::Mat postCanny = cannyPreprocessing(img);
    cv::Mat noWhiteImg = removeWhite(postCanny);
    cv::Mat noBlueImg = removeBlue(noWhiteImg);
    cv::Mat noPostitImg = removePostit(noBlueImg);
    cv::Mat noTicketImg = removeTicket(noPostitImg);

    cv::Mat dilatedImg = imageDilate(noTicketImg);

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

cv::Mat cannyPreprocessing(cv::Mat img) {

    cv::Mat cannyImg;
    cv::Canny(img, cannyImg, 122, 46);
    cv::Mat newPostCanny;
    img.copyTo(newPostCanny, cannyImg);
    return  newPostCanny;

}


cv::Mat removeWhite(cv::Mat img) {


    cv::Mat noWhiteImg;
    cv::inRange(img , cv::Scalar(11, 11, 11), cv::Scalar(255, 255, 255), noWhiteImg);
    img.copyTo(img, 255 - noWhiteImg);
    return  img;
}

cv::Mat removeBlue(cv::Mat img) {


    cv::Mat noBlueImg;
    cv::inRange(img , cv::Scalar(16, 2, 0), cv::Scalar(253, 232, 83), noBlueImg);
    cv::inRange(img , cv::Scalar(120, 95, 69), cv::Scalar(255, 250, 185), noBlueImg);
    img.copyTo(img, 255 - noBlueImg);
    return  img;
}

cv::Mat removePostit(cv::Mat img) {


    cv::Mat noPostitImg;
    cv::inRange(img , cv::Scalar(0, 115, 115), cv::Scalar(110, 255, 255), noPostitImg);
    cv::inRange(img , cv::Scalar(40, 50, 60), cv::Scalar(90, 80, 90), noPostitImg);
    img.copyTo(img, 255 - noPostitImg);
    return  img;
}

cv::Mat removeTicket(cv::Mat img) {


    cv::Mat noTicketImg;
    cv::inRange(img , cv::Scalar(110, 130, 195), cv::Scalar(180, 190, 255), noTicketImg);
    cv::inRange(img , cv::Scalar(0, 65, 110), cv::Scalar(95, 115, 205), noTicketImg);
    img.copyTo(img, 255 - noTicketImg);
    return  img;
}

cv::Mat imageDilate(cv::Mat img) {

    cv::Mat afterKernel;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(8, 8));
    cv::dilate(img, afterKernel, kernel);
    return afterKernel;
}

