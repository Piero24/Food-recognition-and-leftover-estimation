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
#include "../include/imagePreprocessing.h"
#include "../include/performImageSegmentation.h"

#include "../include/firstSegmentator.h"
#include "../include/secondSegmentator.h"
#include "../include/thirdSegmentator.h"

#include "../include/outputCombined.h"

// Pallino giallo:
//     Codice Unicode: \U0001F7E1
//     Valore RGB: (255, 255, 0)
//     Valore BGR: (0, 255, 255)

// Pallino arancione:
//     Codice Unicode: \U0001F7E0
//     Valore RGB: (255, 165, 0)
//     Valore BGR: (0, 165, 255)

// Pallino viola:
//     Codice Unicode: \U0001F7E3
//     Valore RGB: (128, 0, 128)
//     Valore BGR: (128, 0, 128)

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

cv::Mat segmentationOperation(cv::Mat img, int numShowRects, std::vector<cv::Rect> rects);

int main(int argc, char** argv) {

    std::vector<cv::Mat> hcombinedVec;
    std::vector<cv::Mat> trayVector = imgUploader(argc, argv);
   
    size_t ch = trayVector.size();
    for (size_t i = 0; i < ch; i++) {
    //for (size_t i = 0; i < 1; i++) {

        cv::Mat img = trayVector[i].clone();
        cv::Mat imagePreprocessed = segmentationPreprocessing(img);

        std::vector<cv::Rect> rects = recSegmentation(imagePreprocessed, "f");

        // number of region proposals to show
        int numShowRects = rects.size();
        std::cout << "Total Number of Region Proposals: " << numShowRects << std::endl;

        // create a copy of original image
        //cv::Mat imgOut = img.clone();
        cv::Mat tmpImg = imagePreprocessed.clone();


        cv::Mat firstImgOut = firstSegmentationFunc(tmpImg, rects);
        cv::Mat secondImgOut = secondSegmentationFunc(firstImgOut, rects);
        cv::Mat thirdImgOut = thirdSegmentationFunc(secondImgOut, rects);

        //int firstNumShowRects = firstRectVec.size();
        //cv::Mat outImg = segmentationOperation(tmpImg, firstNumShowRects, firstRectVec);


        cv::Mat combined;
        cv::hconcat(img, thirdImgOut, combined);
        //cv::imshow("Task" + std::to_string(i), combined);
        //cv::waitKey(0);
        hcombinedVec.push_back(combined);

    }

    cv::Mat combinedImage = pushOutTray(hcombinedVec);

    cv::imwrite("Comple Tray2.jpg", combinedImage);
    cv::imshow("Comple Tray", combinedImage);
    cv::waitKey(0);

    return 0;
}



cv::Mat segmentationOperation(cv::Mat img, int numShowRects, std::vector<cv::Rect> rects) {

    cv::Mat imOut = img.clone();

    size_t aa = rects.size();
    for (size_t j = 0; j < aa; j++) {

        int areaRect = rects[j].width * rects[j].height;

        if (j < numShowRects) {

            // Total aprox image area 1228800
            if ((areaRect > 50800) && (areaRect  < 428800)) {

                cv::Rect roi = rects[j];
                int nonBlackPixelCount = 0;

                for (int y = roi.y; y < roi.y + roi.height; y++) {
                    for (int x = roi.x; x < roi.x + roi.width; x++) {
                        if (imOut.at<uchar>(y, x) != 0) { // Controllo se il pixel non è nero
                            nonBlackPixelCount++;
                        }
                    }
                }


                double density = static_cast<double>(nonBlackPixelCount) / (roi.width * roi.height);
                //std::cout << "Density: " << density << std::endl;
                
                if ((density < 0.1)) {

                    std::cout << "Density: " << density << std::endl;
                    rectangle(imOut, rects[j], cv::Scalar(0, 255, 0), 2);
                    std::cout << "Rect: " << rects[j] << std::endl;
                }
            }
        }
        else {
            break;
        }
    }

    return imOut;
}