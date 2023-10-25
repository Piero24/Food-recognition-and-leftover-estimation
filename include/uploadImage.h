#ifndef UPLOADIMAGE_H
#define UPLOADIMAGE_H

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/types.hpp>


/**
* Loads a set of images from a specific directory and returns them as an image vector.
* @param trayPath The path to the directory containing the images to load.
* @return An array of images loaded from the directory.
*/
std::vector<cv::Mat> imgUploader(std::string trayPath);

#endif
