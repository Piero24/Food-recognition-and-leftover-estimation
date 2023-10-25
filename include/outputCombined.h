#ifndef OUTPUTCOMBINED_H
#define OUTPUTCOMBINED_H

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/types.hpp>

/**
* Combines a series of images horizontally and moves them up with spacing between them.
* @param hcombinedVec A vector of images to combine and move.
* @return The resulting image with the combined and moved images.
*/
cv::Mat pushOutTray(std::vector<cv::Mat> hcombinedVec);

#endif