#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"


#include "../include/thirdSegmentator.h"

// Pallino viola:
//     Codice Unicode: \U0001F7E3
//     Valore RGB: (128, 0, 128)
//     Valore BGR: (128, 0, 128)

cv::Mat thirdSegmentationFunc(cv::Mat img) {

    std::cout << "\U0001F7E3  WARNING: the thirdSegmentationFunc() method isn't complete" << std::endl;

    return img;
}