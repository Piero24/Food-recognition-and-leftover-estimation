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

cv::Mat segmentationOperation(cv::Mat img, int i);

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

        cv::Mat img = trayVector[i].clone();

        cv::Mat imgOut = segmentationOperation(img, i);

        cv::Mat combined;
        cv::hconcat(img, imgOut, combined);

        cv::imshow("Task" + std::to_string(i), combined);
        cv::waitKey(0);
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



cv::Mat segmentationOperation(cv::Mat img, int i) {
    
    int imageWidth = img.cols;
    int imageHeight = img.rows;
    //std::cout << img.cols * img.rows << std::endl;


    // TODO: BISOGNA LAVORE SUL BLUE YOUGURTH E PANTALONI TRAY5
    // Rimuove blue
    // cv::Scalar lowerBoundBlue(30, 20, 5);
    // cv::Scalar upperBoundBlue(255, 170, 120);

    // cv::Mat maskBlue;
    // cv::inRange(img, lowerBoundBlue, upperBoundBlue, maskBlue);

    // cv::Mat postMaskBlue;
    // img.copyTo(postMaskBlue, 255 - maskBlue);







    // Sbianca
    cv::Mat light;
    double alpha = 1.0;
    int beta = 80;
    img.convertTo(light, -1, alpha, beta);


    // Canny
    cv::Mat cannyImg;
    cv::Canny(light, cannyImg, 70, 80);



    // Tiene su originale solo le cose del canny
    cv::Mat resultAfterCanny;
    cv::bitwise_and(img, img, resultAfterCanny, cannyImg);



    // Rimuove bianco
    cv::Scalar lowerBound(170, 170, 170);
    cv::Scalar upperBound(255, 255, 255);

    cv::Mat mask;
    cv::inRange(resultAfterCanny, lowerBound, upperBound, mask);

    cv::Mat postMask;
    resultAfterCanny.copyTo(postMask, 255 - mask);







    // Rimuove grigio
    lowerBound = cv::Scalar(50, 50, 50);
    upperBound = cv::Scalar(190, 190, 190);

    mask.setTo(cv::Scalar(0, 0, 0));
    cv::inRange(postMask, lowerBound, upperBound, mask);

    cv::Mat finalBeforDilate;
    postMask.copyTo(finalBeforDilate, 255 - mask);






    // Dilatazione finale
    cv::Mat dilatedImg;
    cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7));
    cv::dilate(finalBeforDilate, dilatedImg, kernel2);


    // Rimuove bianco
    lowerBound = cv::Scalar(170, 170, 170);
    upperBound = cv::Scalar(255, 255, 255);

    mask.setTo(cv::Scalar(0, 0, 0));
    cv::inRange(dilatedImg, lowerBound, upperBound, mask);

    postMask.setTo(cv::Scalar(0, 0, 0));
    dilatedImg.copyTo(postMask, 255 - mask);




    // Rimuove grigio
    lowerBound = cv::Scalar(50, 50, 50);
    upperBound = cv::Scalar(180, 180, 180);

    mask.setTo(cv::Scalar(0, 0, 0));
    cv::inRange(postMask, lowerBound, upperBound, mask);

    cv::Mat finalResult;
    postMask.copyTo(finalResult, 255 - mask);



    // Rimuove grigio
    lowerBound = cv::Scalar(0, 0, 0);
    upperBound = cv::Scalar(60, 60, 60);

    mask.setTo(cv::Scalar(0, 0, 0));
    cv::inRange(finalResult, lowerBound, upperBound, mask);


    cv::Mat finalResultTmp;
    finalResult.copyTo(finalResultTmp, 255 - mask);


    cv::Mat kernel3 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    finalResult.setTo(cv::Scalar(0, 0, 0));
    cv::dilate(finalResultTmp, finalResult, kernel2);






    
    //return finalResult;






    // create Selective Search Segmentation Object using default parameters
    cv::Ptr<cv::ximgproc::segmentation::SelectiveSearchSegmentation> ss = cv::ximgproc::segmentation::createSelectiveSearchSegmentation();


    // Creazione delle strategie di segmentazione
    cv::Ptr<cv::ximgproc::segmentation::SelectiveSearchSegmentationStrategyColor> colorStrategy = cv::ximgproc::segmentation::createSelectiveSearchSegmentationStrategyColor();
    cv::Ptr<cv::ximgproc::segmentation::SelectiveSearchSegmentationStrategyTexture> textureStrategy = cv::ximgproc::segmentation::createSelectiveSearchSegmentationStrategyTexture();

    // Creazione della strategia multipla
    cv::Ptr<cv::ximgproc::segmentation::SelectiveSearchSegmentationStrategyMultiple> multipleStrategy = cv::ximgproc::segmentation::createSelectiveSearchSegmentationStrategyMultiple(colorStrategy, textureStrategy);

    // Impostazione della strategia multipla per l'algoritmo di selezione selettiva
    ss -> addStrategy(multipleStrategy);


    // set input image on which we will run segmentation
    ss -> setBaseImage(finalResult);

    std::string dd = "f";

    if (dd == "f") {
        // Switch to fast but low recall Selective Search method
        ss -> switchToSelectiveSearchFast();

    } else {
        // Switch to high recall but slow Selective Search method
        ss -> switchToSelectiveSearchQuality();
    }

    // run selective search segmentation on input image
    std::vector<cv::Rect> rects;
    ss -> process(rects);
    std::cout << "Total Number of Region Proposals: " << rects.size() << std::endl;

    // number of region proposals to show
    int numShowRects = rects.size();

    // create a copy of original image
    //cv::Mat imOut = img.clone();
    cv::Mat imOut = finalResult.clone();

    size_t aa = rects.size();
    for (size_t j = 0; j < aa; j++) {

        int areaRect = rects[j].width * rects[j].height;

        if (j < numShowRects) {
            if ((areaRect > 170800) && (areaRect  < 428800)) {

                int asseX = rects[j].x;
                int asseY = rects[j].y;

                cv::Rect roi(asseX, asseX, asseY, asseY);
                int nonBlackPixelCount = 0;

                for (int y = roi.y; y < roi.y + roi.height; y++) {
                    for (int x = roi.x; x < roi.x + roi.width; x++) {
                        if (finalResult.at<uchar>(y, x) != 0) { // Controllo se il pixel non è nero
                            nonBlackPixelCount++;
                        }
                    }
                }

                double density = static_cast<double>(nonBlackPixelCount) / (roi.width * roi.height);
                std::cout << "Density: " << density << std::endl;

                rectangle(imOut, rects[j], cv::Scalar(0, 255, 0), 2);
                std::cout << "Rect: " << rects[j] << std::endl;
            }
        }
        else {
            break;
        }
    }

    return imOut;
}
