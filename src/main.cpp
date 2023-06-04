// clear && make && ./Food-Recognition ./Food-recognition-and-leftover-estimation/dataset/tray1/

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/uploadImage.h"

#include "../include/prePTester.h"

#include "../include/imagePreprocessing.h"

#include "../include/circularBoundingBoxes.hpp"
#include "../include/rectangularBoundingBoxes.hpp"
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

// Pallino blu:
//     Codice Unicode: \U0001F535
//     Valore RGB: (0, 0, 255)
//     Valore BGR: (255, 0, 0)



cv::Mat boundingBoxTester(cv::Mat img, std::vector<cv::Vec3f> circlesVector, std::vector<cv::Rect> rectanglesVector);


int main(int argc, char** argv) {


    std::vector<cv::Mat> horizontalCombinedVector;
    std::vector<cv::Mat> trayVector = imgUploader(argc, argv);

    // hcombinedVec = multipleTestPreProcessing(trayVector);
    // return 0;
   
    size_t trayVectorSize = trayVector.size();
    for (size_t i = 0; i < trayVectorSize; i++) {

        std::cout << "\n\n######################################## START IMAGE N: " << i + 1 << std::endl;

        cv::Mat img = trayVector[i].clone();
        //cv::Mat imagePreprocessedTest = testPreProcessing(img);
        cv::Mat imagePreprocessed = segmentationPreprocessing(img);

        std::vector<cv::Vec3f> circlesVector = findCircularBoundingBoxes(img);
        std::vector<cv::Rect> rectanglesVector = findRectangularBoundingBoxes(img, imagePreprocessed);

        cv::Mat imgWithBoundingBox = boundingBoxTester(img, circlesVector, rectanglesVector);

        cv::Mat thirdImgOut = thirdSegmentationFunc(imgWithBoundingBox);

        cv::Mat noBackgroundImg = subjectIsolator(img, circlesVector, rectanglesVector);

        cv::Mat combinedTest;
        cv::hconcat(img, thirdImgOut, combinedTest);

        cv::Mat combined;
        cv::hconcat(combinedTest, noBackgroundImg, combined);
        horizontalCombinedVector.push_back(combined);

    }

    cv::Mat combinedImage = pushOutTray(horizontalCombinedVector);

    cv::imwrite("Comple Tray.jpg", combinedImage);
    cv::imshow("Comple Tray", combinedImage);
    cv::waitKey(0);

    return 0;
}


cv::Mat boundingBoxTester(cv::Mat img, std::vector<cv::Vec3f> circlesVector, std::vector<cv::Rect> rectanglesVector){

    cv::Mat clonedImg = img.clone();

    for (const auto& circle : circlesVector) {
        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        cv::circle(clonedImg, center,  cvRound(circle[2]), cv::Scalar(0, 255, 255), 10);
    }

    for (const auto& rect : rectanglesVector) {
        rectangle(clonedImg, rect, cv::Scalar(0, 255, 0), 10);
    }

    return clonedImg;

}





