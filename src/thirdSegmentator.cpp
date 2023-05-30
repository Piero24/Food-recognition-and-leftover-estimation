#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"


#include "../include/thirdSegmentator.h"



cv::Mat thirdSegmentationFunc(cv::Mat img, std::vector<cv::Rect> rects) {

    std::cout << "\u26A0  WARNING: the thirdSegmentationFunc() method isn't complete" << std::endl;

    return img;
}