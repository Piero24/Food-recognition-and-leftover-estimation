#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/outputCombined.h"


cv::Mat pushOutTray(std::vector<cv::Mat> hcombinedVec) {

    int combinedHeight = 0;
    int combinedWidth = hcombinedVec[0].cols;
    int spacing = 15;

    // Calculate the total width of the combined image
    for (const cv::Mat& image : hcombinedVec) {
        combinedHeight += image.rows + spacing;
        combinedWidth = std::max(combinedWidth, image.cols);
    }

   // Create the combined image
    cv::Mat combinedImage(combinedHeight, combinedWidth, CV_8UC3, cv::Scalar(0, 0, 0));

    // Current y position in the combined image
    int currentY = 0;

    // Combining images
    for (const cv::Mat& image : hcombinedVec) {
        // Copy the image to the current position in the combined image
        cv::Rect roi(0, currentY, image.cols, image.rows);
        image.copyTo(combinedImage(roi));

        // Update the current y position
        currentY += image.rows + spacing;
    }

    return combinedImage;
}

