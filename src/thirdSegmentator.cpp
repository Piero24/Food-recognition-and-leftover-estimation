#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"


#include "../include/thirdSegmentator.h"

// Pallino blu:
//     Codice Unicode: \U0001F535
//     Valore RGB: (0, 0, 255)
//     Valore BGR: (255, 0, 0)

cv::Mat thirdSegmentationFunc(cv::Mat img, std::vector<cv::Rect> rects) {

    std::cout << "\u26A0  WARNING: the thirdSegmentationFunc() method isn't complete" << std::endl;

    return img;
}