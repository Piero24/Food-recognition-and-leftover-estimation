// make
// ./Food-Recognition ./Food-recognition-and-leftover-estimation/dataset/tray1/
// ./Food-Recognition ./Food-recognition-and-leftover-estimation/dataset/tray2/

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/uploadImage.h"

#include "../include/prePTester.h"

#include "../include/imagePreprocessing.h"

#include "../include/firstSegmentator.h"
#include "../include/secondSegmentator.h"
#include "../include/thirdSegmentator.h"

#include "../include/detector.h"

#include "../include/outputCombined.h"



// Pallino arancione:
//     Codice Unicode: \U0001F7E0
//     Valore RGB: (255, 165, 0)
//     Valore BGR: (0, 165, 255)

// Pallino nero:
//     Codice Unicode: \U000026AB
//     Valore RGB: (0, 0, 0)
//     Valore BGR: (0, 0, 0)

// Pallino bianco:
//     Codice Unicode: \U000026AA
//     Valore RGB: (255, 255, 255)
//     Valore BGR: (255, 255, 255)

// Pallino marrone:
//     Codice Unicode: \U0001F7E5
//     Valore RGB: (139, 69, 19)
//     Valore BGR: (19, 69, 139)


int main(int argc, char** argv) {

    std::vector<cv::Mat> hcombinedVec;
    std::vector<cv::Mat> trayVector = imgUploader(argc, argv);

    // hcombinedVec = multipleTestPreProcessing(trayVector);
    // return 0;
   
    size_t ch = trayVector.size();
    for (size_t i = 0; i < ch; i++) {
    //for (size_t i = 0; i < 1; i++) {

        std::cout << "\n\n######################################## START IMAGE N: " << i + 1 << std::endl;

        cv::Mat img = trayVector[i].clone();

        //cv::Mat iii = testPreProcessing(img);
        cv::Mat imagePreprocessed = segmentationPreprocessing(img);


        std::vector<cv::Rect> identifiedRegions;

        cv::Mat firstImgOut = firstSegmentationFunc(img);
        //cv::Mat secondImgOut = secondSegmentationFunc(img, iii, identifiedRegions);
        cv::Mat thirdImgOut = thirdSegmentationFunc(firstImgOut);

        cv::Mat noBackgroundImg = subjectIsolator(img, identifiedRegions);

        cv::Mat combined;
        cv::hconcat(img, thirdImgOut, combined);
        //cv::imshow("Task" + std::to_string(i), combined);
        //cv::waitKey(0);
        hcombinedVec.push_back(combined);

    }

    cv::Mat combinedImage = pushOutTray(hcombinedVec);

    cv::imwrite("Comple Tray.jpg", combinedImage);
    cv::imshow("Comple Tray", combinedImage);
    cv::waitKey(0);

    return 0;
}

// 1 ok con riserva
// 2 ok
// 3 no 4
// 4 no 2 
// 5 ok
// 6 ok
// 7 ok
// 8 ok