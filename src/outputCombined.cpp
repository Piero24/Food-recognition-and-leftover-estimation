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

    // Calcola la larghezza totale dell'immagine combinata
    for (const cv::Mat& image : hcombinedVec) {
        combinedHeight += image.rows + spacing;
        combinedWidth = std::max(combinedWidth, image.cols);
    }

    // Crea l'immagine combinata
    cv::Mat combinedImage(combinedHeight, combinedWidth, CV_8UC3, cv::Scalar(0, 0, 0));

    // Posizione y corrente nell'immagine combinata
    int currentY = 0;

    // Combinazione delle immagini
    for (const cv::Mat& image : hcombinedVec) {
        // Copia l'immagine nella posizione corrente nell'immagine combinata
        cv::Rect roi(0, currentY, image.cols, image.rows);
        image.copyTo(combinedImage(roi));

        // Aggiorna la posizione y corrente
        currentY += image.rows + spacing;
    }

    return combinedImage;
}

