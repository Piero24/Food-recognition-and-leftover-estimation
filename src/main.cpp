// clear && make && ./Food-Recognition ./Food-recognition-and-leftover-estimation/dataset

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/uploadImage.h"
#include "../include/imagePreprocessing.h"
#include "../include/circularBoundingBoxes.h"
#include "../include/rectangularBoundingBoxes.h"
#include "../include/outputCombined.h"
#include "../include/detector.h"
#include "../include/classificator.h"
#include "../include/drawerAndTester.h"
#include "../include/foodCategories.h"


int main(int argc, char** argv) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_string>" << std::endl;
        return 1;
    }

    // For apply the original mask to the image for extract feature faster
    // std::string outputDir = argv[1];
    // outputDir += "/features";
    // overlayMasks(argv[1], argv[1] + outputDir);

    for (int trayNumber = 1; trayNumber < 9; trayNumber++) {

        std::vector<cv::Mat> horizontalCombinedVector;
        int numOfBoxesFirstImg = 0;
        std::string trayPath = argv[1];
        trayPath += "/tray" + std::to_string(trayNumber) + "/";

        std::cout << "\n--> START TRAY N°: " << trayNumber << std::endl;

        std::vector<cv::Mat> trayVector = imgUploader(trayPath);
        size_t trayVectorSize = trayVector.size();

        std::vector<StringMatTuple> VecFoodsFirstImage;

        for (size_t i = 0; i < trayVectorSize; i++) {

            std::cout << " Start Image N° " << i + 1 << std::endl;
        
            cv::Mat img = trayVector[i].clone();

            cv::Mat imagePreprocessed = segmentationPreprocessing(img);
            std::vector<cv::Vec3f> circlesVector = findCircularBoundingBoxes(img);
            std::vector<cv::Rect> rectanglesVector = findRectangularBoundingBoxes(img, imagePreprocessed, 0);

            // Recreate the final bounding box by merging the previous methods
            std::vector<std::vector<cv::Point>> foodContours = subjectIsolator(img, circlesVector, rectanglesVector, numOfBoxesFirstImg);
            
            std::string patchPath = argv[1];
            patchPath += "/selectedFeatures/";

            std::vector<Food> VecFoodDetected = foodClassificator(img, foodContours, 2, patchPath, VecFoodsFirstImage);

            // DEBUGGING
            // for (const auto& food : VecFoodDetected) {
            //     std::cout << "    --> Food Found: " << food.name << std::endl;
            // }

            cv::Mat imageWithContours = contourAndBBoxDraw(img, VecFoodDetected, false);

            // DEBUGGING (Change also imageWithContours below with combinedTest)
            // cv::Mat imgWithBoundingBoxandSeg = boundingBoxTester(img, circlesVector, rectanglesVector);

            // cv::Mat combinedTest;
            // cv::hconcat(imgWithBoundingBoxandSeg, imageWithContours, combinedTest);

            cv::Mat combined;
            cv::hconcat(img, imageWithContours, combined);
            horizontalCombinedVector.push_back(combined);
        }

        cv::Mat combinedImage = pushOutTray(horizontalCombinedVector);

        std::string trayName = "Complete Tray " + std::to_string(trayNumber);
        std::string trayNameForSave = trayName + ".jpg";

        cv::imwrite("Results/" + trayNameForSave, combinedImage);
        // cv::imshow(trayName, combinedImage);
        // cv::waitKey(0);
    }

    return 0;
}
