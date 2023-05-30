#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"


#include "../include/secondSegmentator.h"

// Pallino verde:
//     Codice Unicode: \U0001F7E2
//     Valore RGB: (0, 255, 0)
//     Valore BGR: (0, 255, 0)


cv::Mat secondSegmentationFunc(cv::Mat img, std::vector<cv::Rect> rects) {
    
    std::sort(rects.begin(), rects.end(), compareRectangles);

    std::vector<cv::Rect> noToBig = pushOut(rects);

    // Rimuovi i rettangoli interni
    std::vector<cv::Rect> outerRectangles = removeInnerRectangles(noToBig);

    // Stampa i rettangoli esterni
    // for (const cv::Rect& rect2 : outerRectangles) {
    //     std::cout << "Outer Rectangle: " << rect2 << std::endl;
    // }

    int numShowRects = outerRectangles.size();

    cv::Mat imgOut = img.clone();

    size_t aa = outerRectangles.size();
    for (size_t j = 0; j < aa; j++) {

        int areaRect = outerRectangles[j].width * outerRectangles[j].height;

        if (j < numShowRects) {

            // Total aprox image area 1228800
            if ((areaRect > 50800) && (areaRect  < 428800)) {

                cv::Rect roi = outerRectangles[j];
                int nonBlackPixelCount = 0;

                for (int y = roi.y; y < roi.y + roi.height; y++) {
                    for (int x = roi.x; x < roi.x + roi.width; x++) {
                        if (imgOut.at<uchar>(y, x) != 0) {
                            nonBlackPixelCount++;
                        }
                    }
                }


                double density = static_cast<double>(nonBlackPixelCount) / (roi.width * roi.height);
                //std::cout << "Density: " << density << std::endl;
                
                if ((density < 0.1)) {

                    std::cout << "\U0001F7E2" << "  Rect: " << rects[j] << " Density: " << density << std::endl;
                    rectangle(imgOut, outerRectangles[j], cv::Scalar(0, 255, 0), 2);
                }
            }
        }
        else {
            break;
        }
    }

    return imgOut;
}


bool compareRectangles(const cv::Rect& rect1, const cv::Rect& rect2) {
    int area1 = rect1.width * rect1.height;
    int area2 = rect2.width * rect2.height;
    return area1 > area2;
}


// Funzione per controllare se due rettangoli si sovrappongono
bool isOverlap(const cv::Rect& rect1, const cv::Rect& rect2) {
    return (rect1 & rect2).area() > 0;
}


// Funzione per rimuovere i rettangoli interni
std::vector<cv::Rect> removeInnerRectangles(const std::vector<cv::Rect>& rectangles) {
    std::vector<cv::Rect> result;

    for (const cv::Rect& rect : rectangles) {
        bool isInner = false;

        for (const cv::Rect& existingRect : result) {
            if (isOverlap(rect, existingRect)) {
                isInner = true;
                break;
            }
        }

        if (!isInner) {
            result.push_back(rect);
        }
    }

    return result;
}


std::vector<cv::Rect> pushOut(const std::vector<cv::Rect>& rectangles) {

    std::vector<cv::Rect> result;

    for (const cv::Rect& rect : rectangles) {
        int areaRect = rect.width*rect.width;
        //if ((areaRect > 50800) && (areaRect  < 428800)) {
        if (areaRect  < 428800) {

            result.push_back(rect);

        }
        
    }

    return result;
}
