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

    // TEST 1:
    // https://docs.opencv.org/3.4/d7/d1c/tutorial_js_watershed.html

    // TEST 2:
    // Segmento l'immagine con cv::threshold(grayImage, mask, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
    // Pe rimuovere bianco e varie sfumature (Non mi importa se sparisce cibo) uso il cibo rimasto per trovare
    // un cluster per ogni piatto e i rispettivi centroidi (Se piatto vuoto non trova nulla e va bene).
    // Uso i centroidi per trovare i cerchi dei piatti (tipo trova i cerchi possibili con circa queste coordinate per il centro)
    // Una volta trovato i cerchi, per ogni cerchio rimuovo il bianco dei piatto o simile e quello che rimane è cibo anche se poco.
    // Uso quel cibo rimasto (solo i pezzi più grossi scarto pixel singoli e pezzi piccoli) per fare dilate e poi tramite maschera prendere da immagine originale 
    // con cibo ben visibile e eventualmente anche porzioni estra di piatto... rifaccio pulizia e dovrei avere una buona immagine di cibo.
    // Da quello detecto le bounding box.

    return img;
}