#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/prePTester.h"

/*
DATI TROVATI:

Light Alpha: 1.51
Light Beta: 115
No White Lower: 220
No White Upper: 255
BlueB: 255
BlueG: 110
BlueR: 150
Canny 1: 0
Canny 2: 177
Dilate: 47
Gray: 135
*/


/*
DATI MIGLIORI?

Light Alpha: 1.56
Light Beta: 110
No White Lower: 246
No White Upper: 255
BlueB: 255
BlueG: 150
BlueR: 90
Gray: 135
YellowB: 70
YellowG: 230
YellowR: 220
Canny 1: 3.85
Canny 2: 558
Dilate: 69

*/

int is_gray;
const int is_gray_max = 1;

int canny_1;
const int canny_1_max = 1000;

int canny_2;
const int canny_2_max = 1000;

int light_alpha;
const int light_alpha_max = 1000;

int light_beta;
const int light_beta_max = 150;

int no_white_lower;
int no_white_upper;
const int no_white_max = 255;

int no_gray_min = 36;
int no_gray_upper;
int no_gray_max = 203;


int no_blueB_min = 30+light_beta;
int no_blueB;
int no_blueB_max = 255;

int no_blueG_min = 0+light_beta;
int no_blueG;
int no_blueG_max = 150+light_beta;

int no_blueR_min = 0+light_beta;
int no_blueR;
int no_blueR_max = 90+light_beta;

//MIN 0 blue 200 green 180 red
//MAX 25 blue 230 green 220 red

int no_yellowB_min = 0+light_beta;
int no_yellowB;
int no_yellowB_max = 70;

int no_yellowG_min = 90+light_beta;
int no_yellowG;
int no_yellowG_max = 230+light_beta;

int no_yellowR_min = 100+light_beta;
int no_yellowR;
int no_yellowR_max = 220+light_beta;


int dilate;
const int dilate_max = 1000;

cv::Mat imgTmp;
cv::Mat finalImg = cv::Mat::zeros(imgTmp.size(), imgTmp.type());


static void on_trackbar(int, void*) {

    double light_alpha_double = light_alpha * 0.01;
    
    //Aumenti luminosità
    cv::Mat lighterImage;
    imgTmp.convertTo(lighterImage, -1, light_alpha_double, light_beta);
    cv::namedWindow("Light", cv::WINDOW_NORMAL);
    cv::imshow("Light", lighterImage);
    
    //Togli i bianchi
    cv::Mat noWhiteImg;
    cv::inRange(lighterImage, cv::Scalar(no_white_lower, no_white_lower, no_white_lower), cv::Scalar(no_white_upper, no_white_upper, no_white_upper), noWhiteImg);
    cv::Mat postWhite;
    imgTmp.copyTo(postWhite, 255 - noWhiteImg);
    cv::namedWindow("White", cv::WINDOW_NORMAL);
    cv::imshow("White", postWhite);
    cv::Mat grayImage;
    
    //Togli i blu
    cv::Mat noBlueImg;
    cv::inRange(postWhite, cv::Scalar(no_blueB_min, no_blueG_min, no_blueR_min), cv::Scalar(no_blueB, no_blueG, no_blueR), noBlueImg);
    cv::Mat postBlue;
    postWhite.copyTo(postBlue, 255 - noBlueImg);
    cv::namedWindow("Blue", cv::WINDOW_NORMAL);
    cv::imshow("Blue", postBlue);

    if (is_gray) {
        cv::cvtColor(postWhite, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = postWhite.clone();
    }
   
    //Togli i grigi
    cv::Mat noGrayImg;
    cv::inRange(postBlue, cv::Scalar(no_gray_min, no_gray_min, no_gray_min), cv::Scalar(no_gray_upper, no_gray_upper, no_gray_upper), noGrayImg);
    cv::Mat postGray;
    postBlue.copyTo(postGray, 255 - noGrayImg);
    cv::namedWindow("Gray", cv::WINDOW_NORMAL);
    cv::imshow("Gray", postGray);
    
    //Togli i gialli del post-it
    cv::Mat noPostitImg;
    cv::inRange(postGray, cv::Scalar(no_yellowB_min, no_yellowG_min, no_yellowR_min), cv::Scalar(no_yellowB, no_yellowG, no_yellowR), noPostitImg);
    cv::Mat postPostit;
    postGray.copyTo(postPostit, 255 - noPostitImg);
    cv::namedWindow("NoPostit", cv::WINDOW_NORMAL);
    cv::imshow("NoPostit", postPostit);
    
    
    
    //Usare Canny per determinare i contorni delle pietanze
    cv::Mat cannyImg;
    cv::Canny(postPostit, cannyImg, canny_1, canny_2);
    
    cv::Mat postColors;
    cv::bitwise_and(postPostit, postPostit, postColors, cannyImg);
    
    cv::namedWindow("Canny1", cv::WINDOW_NORMAL);
    cv::imshow("Canny1", cannyImg);
    cv::namedWindow("postColors", cv::WINDOW_NORMAL);
    cv::imshow("postColors", postColors);
    
    //Fare dilation per riempire i buchi lasciati dal Canny
    if (dilate % 2 == 0) {dilate = dilate + 1;}
    cv::Mat afterDilate;
    postColors.copyTo(afterDilate, 255 - cannyImg);
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(dilate, dilate));
    cv::dilate(postColors, afterDilate, kernel);
    
    cv::namedWindow("Final", cv::WINDOW_NORMAL);
    cv::imshow("Final", afterDilate);

    

    finalImg = afterDilate.clone();

}


cv::Mat testPreProcessing(cv::Mat img) {

    imgTmp = img.clone();


    cv::namedWindow("Canny", cv::WINDOW_NORMAL);
	//cv::Mat finalImg = cv::Mat::zeros(imgTmp.size(), imgTmp.type());

    //cv::createTrackbar("Gray", "Canny", &is_gray, is_gray_max, on_trackbar);
    cv::createTrackbar("Light Alpha", "Canny", &light_alpha, light_alpha_max, on_trackbar);
    cv::createTrackbar("Light Beta", "Canny", &light_beta, light_beta_max, on_trackbar);
    cv::createTrackbar("WhiteL", "Canny", &no_white_lower, no_white_max, on_trackbar);
    cv::createTrackbar("WhiteU", "Canny", &no_white_upper, no_white_max, on_trackbar);
    cv::createTrackbar("BlueB", "Canny", &no_blueB, no_blueB_max, on_trackbar);
    cv::createTrackbar("BlueG", "Canny", &no_blueG, no_blueG_max, on_trackbar);
    cv::createTrackbar("BlueR", "Canny", &no_blueR, no_blueR_max, on_trackbar);
    cv::createTrackbar("GrayU", "Canny", &no_gray_upper, no_gray_max, on_trackbar);
    cv::createTrackbar("YellowB", "Canny", &no_yellowB, no_yellowB_max, on_trackbar);
    cv::createTrackbar("YellowG", "Canny", &no_yellowG, no_yellowG_max, on_trackbar);
    cv::createTrackbar("YellowR", "Canny", &no_yellowR, no_yellowR_max, on_trackbar);
    cv::createTrackbar("Canny 1", "Canny", &canny_1, canny_1_max, on_trackbar);
    cv::createTrackbar("Canny 2", "Canny", &canny_2, canny_2_max, on_trackbar);
    cv::createTrackbar("Dilate", "Canny", &dilate, dilate_max, on_trackbar);
    



    on_trackbar(0, 0);

    cv::waitKey(0);
    cv::destroyWindow("Canny");

    
    std::cout << "Light Alpha: " << light_alpha * 0.01 << std::endl;
    std::cout << "Light Beta: " << light_beta << std::endl;
    std::cout << "No White Lower: " << no_white_lower << std::endl;
    std::cout << "No White Upper: " << no_white_upper << std::endl;
    std::cout << "BlueB: " << no_blueB << std::endl;
    std::cout << "BlueG: " << no_blueG << std::endl;
    std::cout << "BlueR: " << no_blueR << std::endl;
    std::cout << "Gray: " << no_gray_upper << std::endl;
    std::cout << "YellowB: " << no_yellowB << std::endl;
    std::cout << "YellowG: " << no_yellowG << std::endl;
    std::cout << "YellowR: " << no_yellowR << std::endl;
    std::cout << "Canny 1: " << canny_1 * 0.01 << std::endl;
    std::cout << "Canny 2: " << canny_2 << std::endl;
    std::cout << "Dilate: " << dilate << std::endl;
    
    cv::imshow("out", finalImg);
    cv::waitKey(0);

    return finalImg;
}


