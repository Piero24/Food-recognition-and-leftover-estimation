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

int light_alpha;
const int light_alpha_max = 1000;

int light_beta;
const int light_beta_max = 1000;

int no_white_lower;
int no_white_upper;
const int no_white_max = 255;

int dilate;
const int dilate_max = 1000;





cv::Mat imgTmp;
cv::Mat finalImg = cv::Mat::zeros(imgTmp.size(), imgTmp.type());


static void on_trackbar(int, void*) {

    cv::Mat grayImage;

    if (is_gray) {
        cv::cvtColor(imgTmp, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = imgTmp.clone();
    }



    double light_alpha_double = light_alpha * 0.01;

    cv::Mat lighterImage;
    grayImage.convertTo(lighterImage, -1, light_alpha_double, light_beta);

    cv::Mat cannyImg;
    cv::Canny(lighterImage, cannyImg, canny_1, canny_2);




    // BIANCO NON VA CAPIRE WHY
    cv::Mat noWhiteImg;
    cv::inRange(cannyImg, cv::Scalar(no_white_lower, no_white_lower, no_white_lower), cv::Scalar(no_white_upper, no_white_upper, no_white_upper), noWhiteImg);

    cv::Mat postMask;
    cannyImg.copyTo(postMask, 255 - noWhiteImg);






    if (dilate % 2 == 0) {
        dilate = dilate + 1;
    }

    cv::Mat afterKernel;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(dilate, dilate));
    cv::dilate(postMask, afterKernel, kernel);
    









    cv::Mat inverted_mask;
    cv::bitwise_and(imgTmp, imgTmp, inverted_mask, afterKernel);

    cv::imshow("Canny", inverted_mask);

    finalImg = inverted_mask.clone();

}


cv::Mat testPreProcessing(cv::Mat img) {

    imgTmp = img.clone();

    cv::namedWindow("Canny");





    cv::createTrackbar("Gray", "Canny", &is_gray, is_gray_max, on_trackbar);
    cv::createTrackbar("Light Alpha", "Canny", &light_alpha, light_alpha_max, on_trackbar);
    cv::createTrackbar("Light Beta", "Canny", &light_beta, light_beta_max, on_trackbar);
    cv::createTrackbar("Canny 1", "Canny", &canny_1, canny_1_max, on_trackbar);
    cv::createTrackbar("Canny 2", "Canny", &canny_2, canny_2_max, on_trackbar);
    cv::createTrackbar("NWL", "Canny", &no_white_lower, no_white_max, on_trackbar);
    cv::createTrackbar("NWU", "Canny", &no_white_upper, no_white_max, on_trackbar);
    cv::createTrackbar("Dilate", "Canny", &dilate, dilate_max, on_trackbar);





    on_trackbar(0, 0);

    cv::waitKey(0);
    cv::destroyWindow("Canny");

    std::cout << "Gray: " << is_gray << std::endl;
    std::cout << "Light Alpha: " << light_alpha * 0.01 << std::endl;
    std::cout << "Light Beta: " << light_beta << std::endl;
    std::cout << "Canny 1: " << canny_1 * 0.01 << std::endl;
    std::cout << "Canny 2: " << canny_2 << std::endl;
    std::cout << "No White Lower: " << no_white_lower << std::endl;
    std::cout << "No White Upper: " << no_white_upper << std::endl;
    std::cout << "Dilate: " << dilate << std::endl;








    cv::imshow("out", finalImg);
    cv::waitKey(0);

    return finalImg;
}
