#include <iostream>
#include <locale>

#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"


#include "../include/firstSegmentator.h"

// Pallino giallo:
//     Codice Unicode: \U0001F7E1
//     Valore RGB: (255, 255, 0)
//     Valore BGR: (0, 255, 255)

cv::Mat firstSegmentationFunc(cv::Mat img, std::vector<cv::Rect> rects) {

    std::cout << "\U0001F7E1  WARNING: the thirdSegmentationFunc() method isn't complete" << std::endl;

    return img;

}