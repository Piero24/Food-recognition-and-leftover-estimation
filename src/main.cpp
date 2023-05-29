// make
// ./Food-Recognition ./Food-recognition-and-leftover-estimation/dataset/tray1/
// ./Food-Recognition ./Food-recognition-and-leftover-estimation/dataset/tray2/

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/Tasks.h"


static void help() {
    std::cout << std::endl <<
    "Usage:" << std::endl <<
    "./ssearch input_image (f|q)" << std::endl <<
    "f=fast, q=quality" << std::endl <<
    "Use l to display less rects, m to display more rects, q to quit" << std::endl;
}

int main(int argc, char** argv) {

    std::vector<cv::Mat> trayVector;
	std::vector<cv::String> fileInFolderVec;

    cv::glob(argv[1], fileInFolderVec, false);

    size_t count = fileInFolderVec.size();
    for (size_t i = 0; i < count; i++) {
        if (fileInFolderVec[i].substr(fileInFolderVec[i].size() - 4) == ".jpg" ||
            fileInFolderVec[i].substr(fileInFolderVec[i].size() - 4) == ".png" ||
            fileInFolderVec[i].substr(fileInFolderVec[i].size() - 4) == ".bmp") {

            cv::Mat img = cv::imread(fileInFolderVec[i]);
            trayVector.push_back(img);
        }
    }

    size_t ch = trayVector.size();
    for (size_t i = 0; i < ch; i++) {

        //cv::Mat grayImage;
        //cv::cvtColor(trayVector[i].clone(), grayImage, cv::COLOR_BGR2GRAY);
	    
        cv::Mat blur;
        cv::Mat img = trayVector[i].clone();

        // cv::Mat light = img.clone();
        // double alpha = 1.0;
        // int beta = 100;
        // img.convertTo(light, -1, alpha, beta);

        //GaussianBlur(img, blur, cv::Size(7, 7), 0);
        //GaussianBlur(img, blur, cv::Size(5, 5), 0);

        cv::Mat cannyImg;
        cv::Canny(img, cannyImg, 90, 100);

        cv::Mat mask = 255 - cannyImg;

        // Applica la maschera all'immagine originale
        cv::Mat result;
        cv::bitwise_and(img, img, result, cannyImg);

        // Definisci il range dei pixel bianchi da rimuovere
        cv::Scalar lowerBound(120, 120, 120); // Valori BGR minimi per bianco
        cv::Scalar upperBound(255, 255, 255); // Valori BGR massimi per bianco

        // Crea la maschera dei pixel bianchi nel range specificato
        cv::Mat mask2;
        cv::inRange(result, lowerBound, upperBound, mask2);

        cv::Mat finalResult;
        result.copyTo(finalResult, 255 - mask2);

        GaussianBlur(finalResult, blur, cv::Size(7, 7), 0);

        cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        cv::erode(finalResult, finalResult, kernel2);


        cv::imshow("Task" + std::to_string(i), blur);
        cv::waitKey(0);

        // // create Selective Search Segmentation Object using default parameters
        // cv::Ptr<cv::ximgproc::segmentation::SelectiveSearchSegmentation> ss = cv::ximgproc::segmentation::createSelectiveSearchSegmentation();

        // // set input image on which we will run segmentation
        // ss -> setBaseImage(new_image);

        // std::string dd = "f";

        // if (dd == "f") {
        //     // Switch to fast but low recall Selective Search method
        //     ss -> switchToSelectiveSearchFast();

        // } else if (dd == "q") {
        //     // Switch to high recall but slow Selective Search method
        //     ss -> switchToSelectiveSearchQuality();
        // } else {
        //     // if argument is neither f nor q print help message
        //     help();
        //     return -2;
        // }

        // // run selective search segmentation on input image
        // std::vector<cv::Rect> rects;
        // ss -> process(rects);
        // std::cout << "Total Number of Region Proposals: " << rects.size() << std::endl;

        // // number of region proposals to show
        // int numShowRects = 10;

        //  // create a copy of original image
        // cv::Mat imOut = img.clone();

        // size_t aa = rects.size();
        // for (size_t j = 0; j < aa; j++) {

        //     if (j < numShowRects) {
        //         rectangle(imOut, rects[j], cv::Scalar(0, 255, 0));
        //     }
        //     else {break;}
        // }

        // cv::imshow("Task" + std::to_string(i), imOut);
        // cv::waitKey(0);
    }


    // char in;
    // std::cout << "Press enter to start:" << std::endl;
    // std::cin.get(in);

    // std::cout << "Task 1 press any key to continue." << std::endl;
    // task1(argc, argv);

    // std::cout << "Task 2 press any key to continue." << std::endl;
    // task2(argc, argv);

    // std::cout << "Task 3 press any key to continue." << std::endl;
    // task3(argc, argv);

    // std::cout << "Task 4 press any key to continue." << std::endl;
    // task4(argc, argv);

    // std::cout << "Task 5 press any key to continue." << std::endl;
    // task5(argc, argv);

    // std::cout << "Tasks terminated." << std::endl;

    return 0;
}
