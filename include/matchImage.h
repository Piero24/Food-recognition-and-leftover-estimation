#ifndef MATCHIMAGE_H
#define MATCHIMAGE_H

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/types.hpp>

/**
Match all the images in imgM cropped during the code with the image load in imgL and assign to them a string from imageNames
after n iteration, imgL will be resize with only the images found in the first tray

imgM contain all the images cropped from all trays
imgL contains all the images loaded from loadImage(), and after only the images matched in the first tray
imagesNames contains all the strings loaded in loadImage(), and after only the string associated with the images from imgM
*/
void imgMatching(std::vector<cv::Mat> imgM, std::vector<cv::Mat>& imgL, std::vector<std::string>& imageNames, int n);

/**
Return a vector that conain all the image from the folder imageBase
In the same time, load inside the vector imagesNames all the names of the image from that folder
patchPatch contain the base path to the folder
*/
std::vector<cv::Mat> loadImage(std::string pathPatch, std::vector<std::string>& imageNames);

/**
Return a double vector that contain the percentage with the food that are still in the plate and the start
imgM contain all the images cropped, and after they are ordered by the type
imagesNames associated with the images from imgM, and after ordered as imgM
diff will contain all the different in the end

*/
void leftFood(std::vector<cv::Mat>& imgM, std::vector<std::string>& imageNames,std::vector<double>& diff);

#endif
