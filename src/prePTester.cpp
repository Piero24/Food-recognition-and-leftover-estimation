#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/prePTester.h"

int is_gray;
const int is_gray_max = 1;

int canny_1;
const int canny_1_max = 1000;

int canny_2;
const int canny_2_max = 1000;


cv::Mat imgTmp;
cv::Mat finalImg = cv::Mat::zeros(imgTmp.size(), imgTmp.type());


static void on_trackbar(int, void*) {

    cv::Mat grayImage;

    if (is_gray) {
        cv::cvtColor(imgTmp, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = imgTmp.clone();
    }
    
    cv::Mat cannyImg;
    cv::Canny(grayImage, cannyImg, canny_1, canny_2);
    
    cv::Mat inverted_mask;
    cv::bitwise_and(imgTmp, imgTmp, inverted_mask, cannyImg);







    cv::imshow("Canny", inverted_mask);

    finalImg = inverted_mask.clone();

}


cv::Mat testPreProcessing(cv::Mat img) {

    imgTmp = img.clone();

    cv::namedWindow("Canny");





    cv::createTrackbar("Gray", "Canny", &is_gray, is_gray_max, on_trackbar);
    cv::createTrackbar("Canny 1", "Canny", &canny_1, canny_1_max, on_trackbar);
    cv::createTrackbar("Canny 2", "Canny", &canny_2, canny_2_max, on_trackbar);





    on_trackbar(0, 0);

    cv::waitKey(0);
    cv::destroyWindow("Canny");

    std::cout << "Gray: " << is_gray << std::endl;
    std::cout << "Canny 1: " << canny_1 << std::endl;
    std::cout << "Canny 2: " << canny_2 << std::endl;








    cv::imshow("out", finalImg);
    cv::waitKey(0);

    return finalImg;
}
