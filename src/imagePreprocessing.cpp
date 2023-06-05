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
    
    cv::namedWindow("Img with white", cv::WINDOW_NORMAL);
    cv::imshow("Img with white", postCanny);
    
    cv::Mat noWhiteImg = removeWhite(postCanny);
    
    cv::namedWindow("Img without white", cv::WINDOW_NORMAL);
    cv::imshow("Img without white", noWhiteImg);
    
    cv::Mat noBlueImg = removeBlue(noWhiteImg);
    
    cv::namedWindow("Img without blue", cv::WINDOW_NORMAL);
    cv::imshow("Img without blue", noBlueImg);
    
    cv::Mat noPostitImg = removePostit(noBlueImg);
    cv::namedWindow("Img without post-it", cv::WINDOW_NORMAL);
    cv::imshow("Img without post-it", noPostitImg);

    cv::Mat dilatedImg = imageDilate(noPostitImg);
    cv::namedWindow("Img dil", cv::WINDOW_NORMAL);
    cv::imshow("Img dil", dilatedImg);

    return dilatedImg;
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

    cv::Mat tempImg = img.clone();
    cv::Mat noWhiteImg0, noWhiteImg1, noWhiteImg2, noWhiteImg3, noWhiteImg4, noWhiteImg5;
    cv::inRange(tempImg , cv::Scalar(182, 182, 182), cv::Scalar(255, 255, 255), noWhiteImg0);
    cv::Mat postMask0, postMask1, postMask2, postMask3, postMask4, postMask5;
    tempImg.copyTo(postMask0, 255 - noWhiteImg0);
    cv::inRange(postMask0 , cv::Scalar(142, 142, 142), cv::Scalar(207, 207, 207), noWhiteImg1);
    postMask0.copyTo(postMask1, 255 - noWhiteImg1);
    cv::inRange(postMask1 , cv::Scalar(103, 103, 103), cv::Scalar(161, 161, 161), noWhiteImg2);
    postMask1.copyTo(postMask2, 255 - noWhiteImg2);
    cv::inRange(postMask2 , cv::Scalar(65, 65, 65), cv::Scalar(140, 140, 140), noWhiteImg3);
    postMask2.copyTo(postMask3, 255 - noWhiteImg3);
    cv::inRange(postMask3 , cv::Scalar(52, 52, 52), cv::Scalar(105, 105, 105), noWhiteImg4);
    postMask3.copyTo(postMask4, 255 - noWhiteImg4);
    cv::inRange(postMask4 , cv::Scalar(11, 11, 11), cv::Scalar(79, 79, 79), noWhiteImg5);
    postMask4.copyTo(postMask5, 255 - noWhiteImg5);
    return  postMask5;
}

cv::Mat removeBlue(cv::Mat img) {


    cv::Mat noBlueImg1;
    cv::inRange(img , cv::Scalar(16, 2, 0), cv::Scalar(253, 232, 83), noBlueImg1);
    cv::Mat postMask0 ,postMask1;
    img.copyTo(postMask0, 255 - noBlueImg1);
    cv::Mat noBlueImg2;
    cv::inRange(postMask0 , cv::Scalar(120, 95, 69), cv::Scalar(255, 250, 185), noBlueImg2);
    postMask0.copyTo(postMask1, 255 - noBlueImg2);
    //cv::namedWindow("Img without blue in", cv::WINDOW_NORMAL);
    //cv::imshow("Img without blue in", postMask);
    return  postMask1;
}

cv::Mat removePostit(cv::Mat img) {


    cv::Mat noPostitImg1;
    cv::inRange(img , cv::Scalar(0, 115, 115), cv::Scalar(110, 255, 255), noPostitImg1);
    cv::Mat postMask0;
    img.copyTo(postMask0, 255 - noPostitImg1);
    cv::Mat noPostitImg2;
    cv::inRange(postMask0 , cv::Scalar(40, 50, 60), cv::Scalar(90, 80, 90), noPostitImg2);
    cv::Mat postMask1;
    postMask0.copyTo(postMask1, 255 - noPostitImg2);
    return  postMask1;
}

cv::Mat imageDilate(cv::Mat img) {

    cv::Mat afterKernel;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(9, 9));
    cv::dilate(img, afterKernel, kernel);
    return afterKernel;
}

