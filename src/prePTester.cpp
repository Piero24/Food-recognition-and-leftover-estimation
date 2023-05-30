#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/prePTester.h"



const int alpha_slider_max = 1000;
const int alpha_slider_max_2 = 1000;
int alpha_slider;
int alpha_slider_2;

cv::Mat imgTmp;
cv::Mat cannyImg;
cv::Mat afterCanny;

static void on_trackbar(int, void*) {
    
    cv::Canny(imgTmp, cannyImg, alpha_slider, alpha_slider_2);
    cv::bitwise_and(imgTmp, imgTmp, afterCanny, cannyImg);
    //cv::imshow("Canny", cannyImg);
    cv::imshow("Canny", afterCanny);
}


cv::Mat testPreProcessing(cv::Mat img) {

    imgTmp = img.clone();

    cv::namedWindow("Canny");

    cv::createTrackbar("First ", "Canny", &alpha_slider, alpha_slider_max, on_trackbar);
    cv::createTrackbar("Second ", "Canny", &alpha_slider_2, alpha_slider_max_2, on_trackbar);
    on_trackbar(0, 0);
    cv::waitKey(0);
    cv::destroyWindow("Canny");

    return img;
}
