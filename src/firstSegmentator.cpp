#include <iostream>
#include <locale>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"


#include "../include/firstSegmentator.h"


// Pallino rosso:
//     Codice Unicode: \U0001F534
//     Valore RGB: (255, 0, 0)
//     Valore BGR: (0, 0, 255)


cv::Mat firstSegmentationFunc(cv::Mat img, std::vector<cv::Rect> rects) {

    int numShowRects = rects.size();

    cv::Mat imgOut = img.clone();

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
                        if (imgOut.at<uchar>(y, x) != 0) { // Controllo se il pixel non è nero
                            nonBlackPixelCount++;
                        }
                    }
                }


                double density = static_cast<double>(nonBlackPixelCount) / (roi.width * roi.height);
                //std::cout << "Density: " << density << std::endl;
                
                if ((density < 0.1)) {

                    std::cout << "\U0001F534" << "  Rect: " << rects[j] << " Density: " << density << std::endl;
                    rectangle(imgOut, rects[j], cv::Scalar(0, 0, 255), 2);
                }
            }
        }
        else {
            break;
        }
    }

    return imgOut;
}