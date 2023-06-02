#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/prePTester.h"



cv::Mat startImg0;
cv::Mat startImg1;
cv::Mat startImg2;
cv::Mat startImg3;

cv::Mat finalImg0;
cv::Mat finalImg1;
cv::Mat finalImg2;
cv::Mat finalImg3;

std::vector<cv::Mat> convertToGray(std::vector<cv::Mat> imgVector, int value);
std::vector<cv::Mat> cannyConvert(std::vector<cv::Mat> imgVector, int value1, int value2);
std::vector<cv::Mat> noWhiteOnImg(std::vector<cv::Mat> imgVector, int value1, int value2);
std::vector<cv::Mat> dilateImg(std::vector<cv::Mat> imgVector, int value);
std::vector<cv::Mat> noBlueOnImg(std::vector<cv::Mat> imgVector, int valueB1, int valueB2, int valueG1, int valueG2, int valueR1, int valueR2);
std::vector<cv::Mat> noPostitOnImg(std::vector<cv::Mat> imgVector, int valuePB1, int valuePB2, int valuePG1, int valuePG2, int valuePR1, int valuePR2);
int printShow(std::vector<cv::Mat> imgVector, int value);


int gray0;
int gray1;
int gray2;
int gray3;
int gray_max = 1;


int canny0_1 = 122;
int canny0_2 = 46;
int canny1_1;
int canny1_2;
int canny2_1;
int canny2_2;
int canny3_1;
int canny3_2;
int canny_max = 1000;



int no_white_lower_0 = 182;
int no_white_upper_0 = 255;
int no_white_lower_1 = 142;
int no_white_upper_1 = 207;
int no_white_lower_2 = 103;
int no_white_upper_2 = 161;
int no_white_lower_3 = 65;
int no_white_upper_3 = 140;
int no_white_lower_4 = 52;
int no_white_upper_4 = 105;
int no_white_lower_5 = 11;
int no_white_upper_5 = 79;
int no_white_max = 255;

int no_blueB_lower_0 = 16;
int no_blueB_upper_0 = 253;
int no_blueB_max = 255;

int no_blueG_lower_0 = 2;
int no_blueG_upper_0 = 232;
int no_blueG_max = 255;

int no_blueR_lower_0 = 0;
int no_blueR_upper_0 = 83;
int no_blueR_max = 200;

int no_blueB_lower_1 = 120;
int no_blueB_upper_1 = 255;

int no_blueG_lower_1 = 95;
int no_blueG_upper_1 = 250;

int no_blueR_lower_1 = 69;
int no_blueR_upper_1 = 185;


int no_postitB_lower_0 = 0;
int no_postitB_upper_0 = 110;
int no_postitB_max = 150;

int no_postitG_lower_0 = 115;
int no_postitG_upper_0 = 255;
int no_postitG_max = 255;

int no_postitR_lower_0 = 115;
int no_postitR_upper_0 = 255;
int no_postitR_max = 255;

int no_postitB_lower_1 = 40;
int no_postitB_upper_1 = 90;

int no_postitG_lower_1 = 50;
int no_postitG_upper_1 = 80;

int no_postitR_lower_1 = 60;
int no_postitR_upper_1 = 90;

/*int no_ticketB_lower_0 = 110;
int no_ticketB_upper_0 = 159;
int no_ticketB_max = 200;

int no_ticketG_lower_0 = 135;
int no_ticketG_upper_0 = 195;
int no_ticketG_max = 220;

int no_ticketR_lower_0 = 180;
int no_ticketR_upper_0 = 255;
int no_ticketR_max = 255;*/

int dilate_kernel = 8;
int dilate_kernel_max = 100;




int val = 0;



static void on_trackbar(int, void*) {

    std::vector<cv::Mat> ImgStack;

    cv::Mat newEnterImg0 = startImg0.clone();
    ImgStack.push_back(newEnterImg0);

    if (val) {
        cv::Mat newEnterImg1 = startImg1.clone();
        cv::Mat newEnterImg2 = startImg2.clone();
        cv::Mat newEnterImg3 = startImg3.clone();

        ImgStack.push_back(newEnterImg1);
        ImgStack.push_back(newEnterImg2);
        ImgStack.push_back(newEnterImg3);

    }

    std::vector<cv::Mat> GrayStack0 = convertToGray(ImgStack, gray0);
    std::vector<cv::Mat> CannyStack0 = cannyConvert(GrayStack0, canny0_1, canny0_2);
    std::vector<cv::Mat> NoWhiteStack0 = noWhiteOnImg(CannyStack0, no_white_lower_0, no_white_upper_0);


    

    std::vector<cv::Mat> GrayStack1 = convertToGray(NoWhiteStack0, gray1);
    //std::vector<cv::Mat> CannyStack1 = cannyConvert(GrayStack1, canny1_1, canny1_2);
    std::vector<cv::Mat> NoWhiteStack1 = noWhiteOnImg(GrayStack1, no_white_lower_1, no_white_upper_1);




    std::vector<cv::Mat> GrayStack2 = convertToGray(NoWhiteStack1, gray2);
    //std::vector<cv::Mat> CannyStack2 = cannyConvert(GrayStack2, canny2_1, canny2_2);
    std::vector<cv::Mat> NoWhiteStack2 = noWhiteOnImg(GrayStack2, no_white_lower_2, no_white_upper_2);



    std::vector<cv::Mat> GrayStack3 = convertToGray(NoWhiteStack2, gray3);
    //std::vector<cv::Mat> CannyStack3 = cannyConvert(GrayStack3, canny3_1, canny3_2);
    std::vector<cv::Mat> NoWhiteStack3 = noWhiteOnImg(GrayStack3, no_white_lower_3, no_white_upper_3);



    std::vector<cv::Mat> NoWhiteStack4 = noWhiteOnImg(NoWhiteStack3, no_white_lower_4, no_white_upper_4);
    std::vector<cv::Mat> NoWhiteStack5 = noWhiteOnImg(NoWhiteStack4, no_white_lower_5, no_white_upper_5);


    std::vector<cv::Mat> NoBlueStack0 = noBlueOnImg(NoWhiteStack5, no_blueB_lower_0, no_blueB_upper_0, no_blueG_lower_0, no_blueG_upper_0, no_blueR_lower_0, no_blueR_upper_0);
    
    std::vector<cv::Mat> NoBlueStack1 = noBlueOnImg(NoBlueStack0, no_blueB_lower_1, no_blueB_upper_1, no_blueG_lower_1, no_blueG_upper_1, no_blueR_lower_1, no_blueR_upper_1);
    
    std::vector<cv::Mat> NoPostitStack0 = noPostitOnImg(NoBlueStack1, no_postitB_lower_0, no_postitB_upper_0, no_postitG_lower_0, no_postitG_upper_0, no_postitR_lower_0, no_postitR_upper_0);
    
    std::vector<cv::Mat> NoPostitStack1 = noPostitOnImg(NoPostitStack0, no_postitB_lower_1, no_postitB_upper_1, no_postitG_lower_1, no_postitG_upper_1, no_postitR_lower_1, no_postitR_upper_1);
    
    //std::vector<cv::Mat> NoTicketStack0 = noPostitOnImg(NoPostitStack1, no_ticketB_lower_0, no_ticketB_upper_0, no_ticketG_lower_0, no_ticketG_upper_0, no_ticketR_lower_0, no_ticketR_upper_0);

    //std::vector<cv::Mat> NoBlueStack;
    std::vector<cv::Scalar> colorRanges;
    colorRanges.push_back(cv::Scalar(15, 6, 0));
    colorRanges.push_back(cv::Scalar(175, 155, 141));

    for (const cv::Mat& img : NoWhiteStack5) {
        cv::Mat modifiedImg = img.clone();

        for (int row = 0; row < modifiedImg.rows; row++) {
            for (int col = 0; col < modifiedImg.cols; col++) {
                cv::Vec3b& pixel = modifiedImg.at<cv::Vec3b>(row, col);
                bool isInRange = false;

                size_t range = colorRanges.size();
                for (size_t a = 0; a < range; a++) {
                    cv::Scalar range_min = colorRanges[a];
                    cv::Scalar range_max = colorRanges[a+1];

                    if (pixel[0] >= range_min[0] && pixel[0] <= range_max[0] &&
                        pixel[1] >= range_min[1] && pixel[1] <= range_max[1] &&
                        pixel[2] >= range_min[2] && pixel[2] <= range_max[2]) {
                        isInRange = true;
                        break;
                    }
                }

                if (isInRange) {
                    // Imposta il pixel a zero
                    pixel = cv::Vec3b(0, 0, 0);
                }
            }
        }

        NoPostitStack1.push_back(modifiedImg);
    }




    std::vector<cv::Mat> DilateStack = dilateImg(NoPostitStack1, dilate_kernel);




    printShow(DilateStack, val);
}





std::vector<cv::Mat> multipleTestPreProcessing(std::vector<cv::Mat> imgVector) {

    std::vector<cv::Mat> toReturn;

    startImg0 = imgVector[0].clone();
    startImg1 = imgVector[1].clone();
    startImg2 = imgVector[2].clone();
    startImg3 = imgVector[3].clone();


    cv::namedWindow("Img Zero", cv::WINDOW_NORMAL);
    cv::namedWindow("Img Uno", cv::WINDOW_NORMAL);
    cv::namedWindow("Img Due", cv::WINDOW_NORMAL);
    cv::namedWindow("Img Tre", cv::WINDOW_NORMAL);

    cv::namedWindow("Trackbar", cv::WINDOW_NORMAL);

    // Imposta le dimensioni iniziali della finestra
    //int windowWidth = 100;
    //int windowHeight = 600;
    //cv::resizeWindow("Img Zero", windowWidth, windowHeight);
    //cv::resizeWindow("Img Uno", windowWidth, windowHeight);
    //cv::resizeWindow("Img Due", windowWidth, windowHeight);
    //cv::resizeWindow("Img Tre", windowWidth, windowHeight);

    
    
    
    //cv::createTrackbar("Gray0", "Trackbar", &gray0, gray_max, on_trackbar);
    //cv::createTrackbar("Canny01", "Trackbar", &canny0_1, canny_max, on_trackbar);
    //cv::createTrackbar("Canny02", "Trackbar", &canny0_2, canny_max, on_trackbar);
    //cv::createTrackbar("NWL0", "Trackbar", &no_white_lower_0, no_white_max, on_trackbar);
    //cv::createTrackbar("NWU0", "Trackbar", &no_white_upper_0, no_white_max, on_trackbar);
    





    //cv::createTrackbar("Gray1", "Trackbar", &gray1, gray_max, on_trackbar);
    // cv::createTrackbar("Canny11", "Trackbar", &canny1_1, canny_max, on_trackbar);
    // cv::createTrackbar("Canny12", "Trackbar", &canny1_2, canny_max, on_trackbar);
    //cv::createTrackbar("NWL1", "Trackbar", &no_white_lower_1, no_white_max, on_trackbar);
    //cv::createTrackbar("NWU1", "Trackbar", &no_white_upper_1, no_white_max, on_trackbar);
    
    




    
    //cv::createTrackbar("Gray2", "Trackbar", &gray2, gray_max, on_trackbar);
    // cv::createTrackbar("Canny21", "Trackbar", &canny2_1, canny_max, on_trackbar);
    // cv::createTrackbar("Canny22", "Trackbar", &canny2_2, canny_max, on_trackbar);
    //cv::createTrackbar("NWL2", "Trackbar", &no_white_lower_2, no_white_max, on_trackbar);
    //cv::createTrackbar("NWU2", "Trackbar", &no_white_upper_2, no_white_max, on_trackbar);
    
    



    
    //cv::createTrackbar("Gray3", "Trackbar", &gray3, gray_max, on_trackbar);
    // cv::createTrackbar("Canny31", "Trackbar", &canny3_1, canny_max, on_trackbar);
    // cv::createTrackbar("Canny32", "Trackbar", &canny3_2, canny_max, on_trackbar);
    //cv::createTrackbar("NWL3", "Trackbar", &no_white_lower_3, no_white_max, on_trackbar);
    //cv::createTrackbar("NWU3", "Trackbar", &no_white_upper_3, no_white_max, on_trackbar);


    //cv::createTrackbar("NWL4", "Trackbar", &no_white_lower_4, no_white_max, on_trackbar);
    //cv::createTrackbar("NWU4", "Trackbar", &no_white_upper_4, no_white_max, on_trackbar);

    //cv::createTrackbar("NWL5", "Trackbar", &no_white_lower_5, no_white_max, on_trackbar);
    //cv::createTrackbar("NWU5", "Trackbar", &no_white_upper_5, no_white_max, on_trackbar);
    
    
    cv::createTrackbar("NBBL0", "Trackbar", &no_blueB_lower_0, no_blueB_max, on_trackbar);
    cv::createTrackbar("NBBU0", "Trackbar", &no_blueB_upper_0, no_blueB_max, on_trackbar);
    cv::createTrackbar("NBGL0", "Trackbar", &no_blueG_lower_0, no_blueG_max, on_trackbar);
    cv::createTrackbar("NBGU0", "Trackbar", &no_blueG_upper_0, no_blueG_max, on_trackbar);
    cv::createTrackbar("NBRL0", "Trackbar", &no_blueR_lower_0, no_blueR_max, on_trackbar);
    cv::createTrackbar("NBRU0", "Trackbar", &no_blueR_upper_0, no_blueR_max, on_trackbar);
    
    cv::createTrackbar("NBBL1", "Trackbar", &no_blueB_lower_1, no_blueB_max, on_trackbar);
    cv::createTrackbar("NBBU1", "Trackbar", &no_blueB_upper_1, no_blueB_max, on_trackbar);
    cv::createTrackbar("NBGL1", "Trackbar", &no_blueG_lower_1, no_blueG_max, on_trackbar);
    cv::createTrackbar("NBGU1", "Trackbar", &no_blueG_upper_1, no_blueG_max, on_trackbar);
    cv::createTrackbar("NBRL1", "Trackbar", &no_blueR_lower_1, no_blueR_max, on_trackbar);
    cv::createTrackbar("NBRU1", "Trackbar", &no_blueR_upper_1, no_blueR_max, on_trackbar);
    
    cv::createTrackbar("PostitBL0", "Trackbar", &no_postitB_lower_0, no_postitB_max, on_trackbar);
    cv::createTrackbar("PostitBU0", "Trackbar", &no_postitB_upper_0, no_postitB_max, on_trackbar);
    cv::createTrackbar("PostitGL0", "Trackbar", &no_postitG_lower_0, no_postitG_max, on_trackbar);
    cv::createTrackbar("PostitGU0", "Trackbar", &no_postitG_upper_0, no_postitG_max, on_trackbar);
    cv::createTrackbar("PostitRL0", "Trackbar", &no_postitR_lower_0, no_postitR_max, on_trackbar);
    cv::createTrackbar("PostitRU0", "Trackbar", &no_postitR_upper_0, no_postitR_max, on_trackbar);
    
    cv::createTrackbar("PostitBL1", "Trackbar", &no_postitB_lower_1, no_postitB_max, on_trackbar);
    cv::createTrackbar("PostitBU1", "Trackbar", &no_postitB_upper_1, no_postitB_max, on_trackbar);
    cv::createTrackbar("PostitGL1", "Trackbar", &no_postitG_lower_1, no_postitG_max, on_trackbar);
    cv::createTrackbar("PostitGU1", "Trackbar", &no_postitG_upper_1, no_postitG_max, on_trackbar);
    cv::createTrackbar("PostitRL1", "Trackbar", &no_postitR_lower_1, no_postitR_max, on_trackbar);
    cv::createTrackbar("PostitRU1", "Trackbar", &no_postitR_upper_1, no_postitR_max, on_trackbar);
    
    /*cv::createTrackbar("TicketBL0", "Trackbar", &no_ticketB_lower_0, no_ticketB_max, on_trackbar);
    cv::createTrackbar("TicketBU0", "Trackbar", &no_ticketB_upper_0, no_ticketB_max, on_trackbar);
    cv::createTrackbar("TicketGL0", "Trackbar", &no_ticketG_lower_0, no_ticketG_max, on_trackbar);
    cv::createTrackbar("TicketGU0", "Trackbar", &no_ticketG_upper_0, no_ticketG_max, on_trackbar);
    cv::createTrackbar("TicketRL0", "Trackbar", &no_ticketR_lower_0, no_ticketR_max, on_trackbar);
    cv::createTrackbar("TicketRU0", "Trackbar", &no_ticketR_upper_0, no_ticketR_max, on_trackbar);*/

    cv::createTrackbar("Dilate", "Trackbar", &dilate_kernel, dilate_kernel_max, on_trackbar);





    


    on_trackbar(0, 0);
    cv::waitKey(0);

    std::cout << "Gray: " << gray1<< std::endl;
    
    return toReturn;
}



cv::Mat testPreProcessing(cv::Mat img) {

    startImg0 = img.clone();



    cv::namedWindow("Img Zero", cv::WINDOW_NORMAL);


    cv::namedWindow("Trackbar");

    // Imposta le dimensioni iniziali della finestra
    int windowWidth = 100;
    int windowHeight = 600;
    cv::resizeWindow("Img Zero", windowWidth, windowHeight);


    
    
    
    cv::createTrackbar("Gray0", "Trackbar", &gray0, gray_max, on_trackbar);
    cv::createTrackbar("Canny01", "Trackbar", &canny0_1, canny_max, on_trackbar);
    cv::createTrackbar("Canny02", "Trackbar", &canny0_2, canny_max, on_trackbar);
    cv::createTrackbar("NWL0", "Trackbar", &no_white_lower_0, no_white_max, on_trackbar);
    cv::createTrackbar("NWU0", "Trackbar", &no_white_upper_0, no_white_max, on_trackbar);
    cv::createTrackbar("NBBL0", "Trackbar", &no_blueB_lower_0, no_blueB_max, on_trackbar);
    cv::createTrackbar("NBBU0", "Trackbar", &no_blueB_upper_0, no_blueB_max, on_trackbar);
    cv::createTrackbar("NBGL0", "Trackbar", &no_blueG_lower_0, no_blueG_max, on_trackbar);
    cv::createTrackbar("NBGU0", "Trackbar", &no_blueG_upper_0, no_blueG_max, on_trackbar);
    cv::createTrackbar("NBRL0", "Trackbar", &no_blueR_lower_0, no_blueR_max, on_trackbar);
    cv::createTrackbar("NBRU0", "Trackbar", &no_blueR_upper_0, no_blueR_max, on_trackbar);
    





    cv::createTrackbar("Gray1", "Trackbar", &gray1, gray_max, on_trackbar);
    // cv::createTrackbar("Canny11", "Trackbar", &canny1_1, canny_max, on_trackbar);
    // cv::createTrackbar("Canny12", "Trackbar", &canny1_2, canny_max, on_trackbar);
    cv::createTrackbar("NWL1", "Trackbar", &no_white_lower_1, no_white_max, on_trackbar);
    cv::createTrackbar("NWU1", "Trackbar", &no_white_upper_1, no_white_max, on_trackbar);
    
    




    
    cv::createTrackbar("Gray2", "Trackbar", &gray2, gray_max, on_trackbar);
    // cv::createTrackbar("Canny21", "Trackbar", &canny2_1, canny_max, on_trackbar);
    // cv::createTrackbar("Canny22", "Trackbar", &canny2_2, canny_max, on_trackbar);
    cv::createTrackbar("NWL2", "Trackbar", &no_white_lower_2, no_white_max, on_trackbar);
    cv::createTrackbar("NWU2", "Trackbar", &no_white_upper_2, no_white_max, on_trackbar);
    
    



    
    cv::createTrackbar("Gray3", "Trackbar", &gray3, gray_max, on_trackbar);
    // cv::createTrackbar("Canny31", "Trackbar", &canny3_1, canny_max, on_trackbar);
    // cv::createTrackbar("Canny32", "Trackbar", &canny3_2, canny_max, on_trackbar);
    cv::createTrackbar("NWL3", "Trackbar", &no_white_lower_3, no_white_max, on_trackbar);
    cv::createTrackbar("NWU3", "Trackbar", &no_white_upper_3, no_white_max, on_trackbar);


    cv::createTrackbar("NWL4", "Trackbar", &no_white_lower_4, no_white_max, on_trackbar);
    cv::createTrackbar("NWU4", "Trackbar", &no_white_upper_4, no_white_max, on_trackbar);

    cv::createTrackbar("NWL5", "Trackbar", &no_white_lower_5, no_white_max, on_trackbar);
    cv::createTrackbar("NWU5", "Trackbar", &no_white_upper_5, no_white_max, on_trackbar);

    //cv::createTrackbar("NBL", "Trackbar", &no_blue_lower, no_blue_max, on_trackbar);
    //cv::createTrackbar("NBU", "Trackbar", &no_blue_upper, no_blue_max, on_trackbar);

    cv::createTrackbar("Dilate", "Trackbar", &dilate_kernel, dilate_kernel_max, on_trackbar);






    on_trackbar(0, 0);


    std::cout << "Gray: " << gray1<< std::endl;
    
    return finalImg0;
}




































std::vector<cv::Mat> convertToGray(std::vector<cv::Mat> imgVector, int value) {

    std::vector<cv::Mat> VecToReturn;

    size_t ch = imgVector.size();
    for (size_t i = 0; i < ch; i++) {

        cv::Mat grayImage;

        if (value) {
            cv::cvtColor(imgVector[i], grayImage, cv::COLOR_BGR2GRAY);

        } else {
            grayImage = imgVector[i].clone();
        }

        VecToReturn.push_back(grayImage);

    }

    return VecToReturn;

}


std::vector<cv::Mat> cannyConvert(std::vector<cv::Mat> imgVector, int value1, int value2) {

    std::vector<cv::Mat> VecToReturn;

    size_t ch = imgVector.size();
    for (size_t i = 0; i < ch; i++) {

        cv::Mat tempImg = imgVector[i].clone();

        cv::Mat cannyImg;
        cv::Canny(tempImg, cannyImg, value1, value2);
        cv::Mat newPostCanny;
        tempImg.copyTo(newPostCanny, cannyImg);

        VecToReturn.push_back(newPostCanny);

    }

    return VecToReturn;
}



std::vector<cv::Mat> noWhiteOnImg(std::vector<cv::Mat> imgVector, int value1, int value2) {

    std::vector<cv::Mat> VecToReturn;

    size_t ch = imgVector.size();
    for (size_t i = 0; i < ch; i++) {

        cv::Mat tempImg = imgVector[i].clone();

        cv::Mat noWhiteImg;
        cv::inRange(tempImg , cv::Scalar(value1, value1, value1), cv::Scalar(value2, value2, value2), noWhiteImg);
        cv::Mat postMask;
        tempImg.copyTo(postMask, 255 - noWhiteImg);

        VecToReturn.push_back(postMask);

    }

    return VecToReturn;
}




std::vector<cv::Mat> dilateImg(std::vector<cv::Mat> imgVector, int value) {

    std::vector<cv::Mat> VecToReturn;

    size_t ch = imgVector.size();
    for (size_t i = 0; i < ch; i++) {

        cv::Mat tempImg = imgVector[i].clone();

        if (value % 2 == 0) {
            value = value + 1;
        }

        cv::Mat afterKernel;
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(value, value));
        cv::dilate(tempImg, afterKernel, kernel);

        VecToReturn.push_back(afterKernel);

    }

    return VecToReturn;
}


std::vector<cv::Mat> noBlueOnImg(std::vector<cv::Mat> imgVector, int valueB1, int valueB2, int valueG1, int valueG2, int valueR1, int valueR2) {

     std::vector<cv::Mat> VecToReturn;
     
     size_t ch = imgVector.size();
     for (size_t i = 0; i < ch; i++) {

        cv::Mat tempImg = imgVector[i].clone();

         // Creazione della maschera per il canale blu
         cv::Mat noBlueImg;
	 cv::inRange(tempImg , cv::Scalar(valueB1, valueG1, valueR1), cv::Scalar(valueB2, valueG2, valueR2), noBlueImg);
	 cv::Mat postMask;
         tempImg.copyTo(postMask, 255 - noBlueImg);

        VecToReturn.push_back(postMask);

     }

     return VecToReturn;
 }

std::vector<cv::Mat> noPostitOnImg(std::vector<cv::Mat> imgVector, int valuePB1, int valuePB2, int valuePG1, int valuePG2, int valuePR1, int valuePR2){

     std::vector<cv::Mat> VecToReturn;
     
     size_t ch = imgVector.size();
     for (size_t i = 0; i < ch; i++) {

        cv::Mat tempImg = imgVector[i].clone();

         // Creazione della maschera per il postit
         cv::Mat noPostitImg;
	 cv::inRange(tempImg , cv::Scalar(valuePB1, valuePG1, valuePR1), cv::Scalar(valuePB2, valuePG2, valuePR2), noPostitImg);
	 cv::Mat postMask;
         tempImg.copyTo(postMask, 255 - noPostitImg);

        VecToReturn.push_back(postMask);

     }

     return VecToReturn;
 }

int printShow(std::vector<cv::Mat> imgVector, int value) {

    if (value) {
        finalImg0 = imgVector[0].clone();
        finalImg1 = imgVector[1].clone();
        finalImg2 = imgVector[2].clone();
        finalImg3 = imgVector[3].clone();
        cv::imshow("Img Zero", finalImg0);
        cv::imshow("Img Uno", finalImg1);
        cv::imshow("Img Due", finalImg2);
        cv::imshow("Img Tre", finalImg3);

    } else {
        finalImg0 = imgVector[0].clone();
        cv::imshow("Img Zero", finalImg0);
    }

    return 0;

}



































//------------------------------------------------------------------------------------------------------------------------------------------------------------------TEST DI GIO
// /*
// DATI TROVATI:

// Light Alpha: 1.51
// Light Beta: 115
// No White Lower: 220
// No White Upper: 255
// BlueB: 255
// BlueG: 110
// BlueR: 150
// Canny 1: 0
// Canny 2: 177
// Dilate: 47
// Gray: 135
// */


// /*
// DATI MIGLIORI?

// Light Alpha: 1.56
// Light Beta: 110
// No White Lower: 246
// No White Upper: 255
// BlueB: 255
// BlueG: 150
// BlueR: 90
// Gray: 135
// YellowB: 70
// YellowG: 230
// YellowR: 220
// Canny 1: 3.85
// Canny 2: 558
// Dilate: 69

// */

// int is_gray;
// const int is_gray_max = 1;

// int canny_1 = 62;
// const int canny_1_max = 1000;

// int canny_2 = 354;
// const int canny_2_max = 1000;

// int light_alpha = 325;
// const int light_alpha_max = 1000;

// int light_beta = 104;
// const int light_beta_max = 150;

// int no_white_lower = 208;
// int no_white_upper = 255;
// const int no_white_max = 255;

// int no_gray_min = 36;
// int no_gray_upper = 80;
// int no_gray_max = 203;


// int no_blueB_min = 30+light_beta;
// int no_blueB = 255;
// int no_blueB_max = 255;

// int no_blueG_min = 0+light_beta;
// int no_blueG = 150;
// int no_blueG_max = 150+light_beta;

// int no_blueR_min = 0+light_beta;
// int no_blueR = 90;
// int no_blueR_max = 90+light_beta;

// //MIN 0 blue 200 green 180 red
// //MAX 25 blue 230 green 220 red

// int no_yellowB_min = 0+light_beta;
// int no_yellowB = 70;
// int no_yellowB_max = 70;

// int no_yellowG_min = 90+light_beta;
// int no_yellowG = 230;
// int no_yellowG_max = 230+light_beta;

// int no_yellowR_min = 100+light_beta;
// int no_yellowR = 220;
// int no_yellowR_max = 220+light_beta;


// int dilate = 17;
// const int dilate_max = 1000;

// cv::Mat imgTmp;
// cv::Mat finalImg = cv::Mat::zeros(imgTmp.size(), imgTmp.type());


// static void on_trackbar(int, void*) {

//     double light_alpha_double = light_alpha * 0.01;
    
//     //Aumenti luminosità
//     cv::Mat lighterImage;
//     imgTmp.convertTo(lighterImage, -1, light_alpha_double, light_beta);

//     //Togli i bianchi
//     cv::Mat noWhiteImg;
//     cv::inRange(lighterImage, cv::Scalar(no_white_lower, no_white_lower, no_white_lower), cv::Scalar(no_white_upper, no_white_upper, no_white_upper), noWhiteImg);
//     cv::Mat postWhite;
//     imgTmp.copyTo(postWhite, 255 - noWhiteImg);

//     cv::Mat grayImage;
    
//     //Togli i blu
//     cv::Mat noBlueImg;
//     cv::inRange(postWhite, cv::Scalar(no_blueB_min, no_blueG_min, no_blueR_min), cv::Scalar(no_blueB, no_blueG, no_blueR), noBlueImg);
//     cv::Mat postBlue;
//     postWhite.copyTo(postBlue, 255 - noBlueImg);


//     if (is_gray) {
//         cv::cvtColor(postWhite, grayImage, cv::COLOR_BGR2GRAY);
//     } else {
//         grayImage = postWhite.clone();
//     }
   
//     //Togli i grigi
//     cv::Mat noGrayImg;
//     cv::inRange(postBlue, cv::Scalar(no_gray_min, no_gray_min, no_gray_min), cv::Scalar(no_gray_upper, no_gray_upper, no_gray_upper), noGrayImg);
//     cv::Mat postGray;
//     postBlue.copyTo(postGray, 255 - noGrayImg);

    
//     //Togli i gialli del post-it
//     cv::Mat noPostitImg;
//     cv::inRange(postGray, cv::Scalar(no_yellowB_min, no_yellowG_min, no_yellowR_min), cv::Scalar(no_yellowB, no_yellowG, no_yellowR), noPostitImg);
//     cv::Mat postPostit;
//     postGray.copyTo(postPostit, 255 - noPostitImg);

    
    
    
//     //Usare Canny per determinare i contorni delle pietanze
//     cv::Mat cannyImg;
//     cv::Canny(postPostit, cannyImg, canny_1, canny_2);
    
//     cv::Mat postColors;
//     cv::bitwise_and(postPostit, postPostit, postColors, cannyImg);
    

//     //Fare dilation per riempire i buchi lasciati dal Canny
//     if (dilate % 2 == 0) {dilate = dilate + 1;}
//     cv::Mat afterDilate;
//     postColors.copyTo(afterDilate, 255 - cannyImg);
//     cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(dilate, dilate));
//     cv::dilate(postColors, afterDilate, kernel);
    
//     // cv::namedWindow("Final", cv::WINDOW_NORMAL);
//     // cv::imshow("Final", afterDilate);

    

//     finalImg = afterDilate.clone();

// }


// cv::Mat testPreProcessing(cv::Mat img) {

//     imgTmp = img.clone();


//     cv::namedWindow("Canny", cv::WINDOW_NORMAL);
// 	//cv::Mat finalImg = cv::Mat::zeros(imgTmp.size(), imgTmp.type());

//     //cv::createTrackbar("Gray", "Canny", &is_gray, is_gray_max, on_trackbar);
//     cv::createTrackbar("Light Alpha", "Canny", &light_alpha, light_alpha_max, on_trackbar);
//     cv::createTrackbar("Light Beta", "Canny", &light_beta, light_beta_max, on_trackbar);
//     cv::createTrackbar("WhiteL", "Canny", &no_white_lower, no_white_max, on_trackbar);
//     cv::createTrackbar("WhiteU", "Canny", &no_white_upper, no_white_max, on_trackbar);
//     cv::createTrackbar("BlueB", "Canny", &no_blueB, no_blueB_max, on_trackbar);
//     cv::createTrackbar("BlueG", "Canny", &no_blueG, no_blueG_max, on_trackbar);
//     cv::createTrackbar("BlueR", "Canny", &no_blueR, no_blueR_max, on_trackbar);
//     cv::createTrackbar("GrayU", "Canny", &no_gray_upper, no_gray_max, on_trackbar);
//     cv::createTrackbar("YellowB", "Canny", &no_yellowB, no_yellowB_max, on_trackbar);
//     cv::createTrackbar("YellowG", "Canny", &no_yellowG, no_yellowG_max, on_trackbar);
//     cv::createTrackbar("YellowR", "Canny", &no_yellowR, no_yellowR_max, on_trackbar);
//     cv::createTrackbar("Canny 1", "Canny", &canny_1, canny_1_max, on_trackbar);
//     cv::createTrackbar("Canny 2", "Canny", &canny_2, canny_2_max, on_trackbar);
//     cv::createTrackbar("Dilate", "Canny", &dilate, dilate_max, on_trackbar);
    



//     on_trackbar(0, 0);

//     //cv::waitKey(0);
//     cv::destroyWindow("Canny");

    
//     std::cout << "Light Alpha: " << light_alpha * 0.01 << std::endl;
//     std::cout << "Light Beta: " << light_beta << std::endl;
//     std::cout << "No White Lower: " << no_white_lower << std::endl;
//     std::cout << "No White Upper: " << no_white_upper << std::endl;
//     std::cout << "BlueB: " << no_blueB << std::endl;
//     std::cout << "BlueG: " << no_blueG << std::endl;
//     std::cout << "BlueR: " << no_blueR << std::endl;
//     std::cout << "Gray: " << no_gray_upper << std::endl;
//     std::cout << "YellowB: " << no_yellowB << std::endl;
//     std::cout << "YellowG: " << no_yellowG << std::endl;
//     std::cout << "YellowR: " << no_yellowR << std::endl;
//     std::cout << "Canny 1: " << canny_1 * 0.01 << std::endl;
//     std::cout << "Canny 2: " << canny_2 << std::endl;
//     std::cout << "Dilate: " << dilate << std::endl;
    
//     //cv::imshow("out", finalImg);
//     //cv::waitKey(0);

//     return finalImg;
// }
























//------------------------------------------------------------------------------------------------------------------------------------------------------------------TEST MIO


// int is_gray;
// const int is_gray_max = 1;

// int canny_1 = 25;
// const int canny_1_max = 1000;

// int canny_2 = 95;
// const int canny_2_max = 1000;

// int light_alpha = 102;
// const int light_alpha_max = 1000;

// int light_beta = 0;
// const int light_beta_max = 1000;

// int no_white_lower = 160;
// int no_white_upper = 255;
// const int no_white_max = 255;

// int dilate = 6;
// const int dilate_max = 500;

// int no_gray_min = 36;
// int no_gray_upper = 117;
// int no_gray_max = 203;




// cv::Mat imgTmp;
// cv::Mat finalImg = cv::Mat::zeros(imgTmp.size(), imgTmp.type());


// static void on_trackbar(int, void*) {

//     cv::Mat newEnterImg = imgTmp.clone();


//     cv::Mat grayImage;

//     if (is_gray) {
//         cv::cvtColor(newEnterImg, grayImage, cv::COLOR_BGR2GRAY);
//     } else {
//         grayImage = newEnterImg.clone();
//     }


//     cv::Mat lighterImage;
//     double light_alpha_double = light_alpha * 0.01;
//     grayImage.convertTo(lighterImage, -1, light_alpha_double, light_beta);



//     cv::Mat noWhiteImg;
//     cv::inRange(lighterImage, cv::Scalar(no_white_lower, no_white_lower, no_white_lower), cv::Scalar(no_white_upper, no_white_upper, no_white_upper), noWhiteImg);
//     cv::Mat postMask;
//     lighterImage.copyTo(postMask, 255 - noWhiteImg);




//     cv::Mat cannyImg;
//     cv::Canny(postMask, cannyImg, canny_1, canny_2);
//     cv::Mat newPostMask;
//     postMask.copyTo(newPostMask, cannyImg);


//     if (dilate % 2 == 0) {
//         dilate = dilate + 1;
//     }

//     cv::Mat afterKernel;
//     cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(dilate, dilate));
//     cv::dilate(newPostMask, afterKernel, kernel);


//     cv::Mat noGrayImg;
//     cv::inRange(afterKernel, cv::Scalar(90, 100, 100), cv::Scalar(190, 180, 170), noGrayImg);
//     cv::Mat postGrayMask;
//     afterKernel.copyTo(postGrayMask, 255 - noGrayImg);

//     cv::Mat noGrayImg2;
//     cv::inRange(postGrayMask, cv::Scalar(no_gray_min, no_gray_min, no_gray_min), cv::Scalar(no_gray_upper, no_gray_upper, no_gray_upper), noGrayImg2);
//     cv::Mat postGray;
//     postGrayMask.copyTo(postGray, 255 - noGrayImg2);


//     cv::Mat noBeigeImg;
//     cv::inRange(postGray, cv::Scalar(130, 150, 170), cv::Scalar(166, 190, 205), noBeigeImg);
//     cv::Mat postBeigeMask;
//     postGray.copyTo(postBeigeMask, 255 - noBeigeImg);


// // cv::Mat noBeigeImg;
// //     cv::inRange(postGray, cv::Scalar(80, 100, 115), cv::Scalar(166, 190, 205), noBeigeImg);
// //     cv::Mat postBeigeMask;
// //     postGray.copyTo(postBeigeMask, 255 - noBeigeImg);






//     // cv::Mat inverted_mask;
//     // cv::bitwise_and(imgTmp, imgTmp, inverted_mask, afterKernel);

//     finalImg = postBeigeMask.clone();
//     cv::imshow("CannyImg", finalImg);
// }


// cv::Mat testPreProcessing(cv::Mat img) {

//     imgTmp = img.clone();

//     cv::namedWindow("Canny");
//     cv::namedWindow("CannyImg");



//     cv::createTrackbar("Gray", "Canny", &is_gray, is_gray_max, on_trackbar);
//     cv::createTrackbar("Light Alpha", "Canny", &light_alpha, light_alpha_max, on_trackbar);
//     cv::createTrackbar("Light Beta", "Canny", &light_beta, light_beta_max, on_trackbar);
//     cv::createTrackbar("NWL", "Canny", &no_white_lower, no_white_max, on_trackbar);
//     cv::createTrackbar("NWU", "Canny", &no_white_upper, no_white_max, on_trackbar);
//     cv::createTrackbar("Canny 1", "Canny", &canny_1, canny_1_max, on_trackbar);
//     cv::createTrackbar("Canny 2", "Canny", &canny_2, canny_2_max, on_trackbar);
//     cv::createTrackbar("Dilate", "Canny", &dilate, dilate_max, on_trackbar);
//     cv::createTrackbar("GrayU", "Canny", &no_gray_upper, no_gray_max, on_trackbar);





//     on_trackbar(0, 0);

//     cv::waitKey(0);
//     cv::destroyWindow("Canny");
//     cv::destroyWindow("CannyImg");

//     std::cout << "Gray: " << is_gray << std::endl;
//     std::cout << "Light Alpha: " << light_alpha * 0.01 << std::endl;
//     std::cout << "Light Beta: " << light_beta << std::endl;
//     std::cout << "No White Lower: " << no_white_lower << std::endl;
//     std::cout << "No White Upper: " << no_white_upper << std::endl;
//     std::cout << "Canny 1: " << canny_1 * 0.01 << std::endl;
//     std::cout << "Canny 2: " << canny_2 << std::endl;
//     std::cout << "Dilate: " << dilate << std::endl;
    








//     //cv::imshow("out", finalImg);
//     //cv::waitKey(0);

//     return finalImg;
// }
