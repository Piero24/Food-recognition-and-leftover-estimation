// clear && make && ./Food-Recognition ./Food-recognition-and-leftover-estimation/dataset/tray1/

#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include "../include/uploadImage.h"

#include "../include/prePTester.h"

#include "../include/imagePreprocessing.h"

#include "../include/circularBoundingBoxes.hpp"
#include "../include/rectangularBoundingBoxes.hpp"
#include "../include/thirdSegmentator.h"

#include "../include/detector.h"

#include "../include/outputCombined.h"



// Pallino arancione:
//     Codice Unicode: \U0001F7E0
//     Valore RGB: (255, 165, 0)
//     Valore BGR: (0, 165, 255)

// Pallino nero:
//     Codice Unicode: \U000026AB
//     Valore RGB: (0, 0, 0)
//     Valore BGR: (0, 0, 0)

// Pallino bianco:
//     Codice Unicode: \U000026AA
//     Valore RGB: (255, 255, 255)
//     Valore BGR: (255, 255, 255)

// Pallino marrone:
//     Codice Unicode: \U0001F7E5
//     Valore RGB: (139, 69, 19)
//     Valore BGR: (19, 69, 139)

// Pallino blu:
//     Codice Unicode: \U0001F535
//     Valore RGB: (0, 0, 255)
//     Valore BGR: (255, 0, 0)



cv::Mat boundingBoxSegmentationTester(cv::Mat img, Detector detectorVec, std::vector<cv::Rect> finalBBoxVec);


int main(int argc, char** argv) {


    std::vector<cv::Mat> horizontalCombinedVector;
    std::vector<cv::Mat> trayVector = imgUploader(argc, argv);

    // VALIDO SOLO PER DEBUG
    // hcombinedVec = multipleTestPreProcessing(trayVector);
    // return 0;
    cv::Mat image0Preprocessed = segmentationPreprocessing(trayVector[0]);
    int numOfBoxes = findRectangularBoundingBoxes(trayVector[0], image0Preprocessed, 1).size();
    
    size_t trayVectorSize = trayVector.size();
    for (size_t i = 0; i < trayVectorSize; i++) {

        std::cout << "\n\n######################################## START IMAGE N: " << i + 1 << std::endl;
	
        cv::Mat img = trayVector[i].clone();

        //cv::Mat imagePreprocessed = testPreProcessing(img);
        cv::Mat imagePreprocessed = segmentationPreprocessing(img);
	    //cv::imshow("Prep",imagePreprocessed);
        std::vector<cv::Vec3f> circlesVector = findCircularBoundingBoxes(img);
        std::vector<cv::Rect> rectanglesVector = findRectangularBoundingBoxes(img, imagePreprocessed, 0);
        
        // TODO (MAYBE)
        cv::Mat thirdImgOut = thirdSegmentationFunc(img);

        // Ricrea la bounding box finale mergiando i metodi precedenti
        Detector detector;
        Detector detectorVec;
        detectorVec = detector.subjectIsolator(img, circlesVector, rectanglesVector);
        std::vector<cv::Rect> finalBBoxVec = detector.fromSegmentationToBBox(img, detectorVec, numOfBoxes);
	
        /*
        

                        NON MODIFICARE E CARICARE MAI IL MAIN SU GITHUB SENZA AVVISARE (POSSIBILMENTE EVITARE DI MODIFICARLO
                                                 -----


        */
        

        // TODO (QUANDO TUTTO QUELLO SOPRA E' FINITO):
        //
        //  - Capire la tipologia di cibo (vettore con struttura dati con tipo cibo quantità e forma (posizione dimensione ecc))
        //  - Disegnare una immagine con bounding box con scritto il tipo di cibo e la quantità (es: 100% 40% ecc o se chiede di farlo in modo specifico seguire indicazioni)
        //  - Disegnare maschera unica con tutti i cibi dell'immagine
        //  - Unire le 2 immagini orizzontalmente con quella originale così da avere in output una riga con immagine vuota immagine con bounding box e cibo e immagine con segmentazione
        //  - Mettere in un vettore (sostanzialmente che output venga come ora)
        //  -
        //  - 
        //  - VERIFICARE CHE BOUNDING BOX, MASCHERE ECC COINCIDANO (IL PIU' POSSIBILE) CON QUELLE FORNITE PER I TEST
        //  - Scrivere codice più presentabile, eliminare quello che non serve (eccetto cose di debug) e commentare (in inglese) le varie chiamate a funzione
        //  - Commentare le funzioni negli header
        //  - meglio file .hpp o .h? Meglio fare classi?
        //  - Testare con altre immagini non presenti nel dataset fornito
        //  - 
        //  - 



        // DA COMMENTARE FINITO TUTTO VALIDO SOLO PER DEBUG
        cv::Mat imgWithBoundingBoxandSeg = boundingBoxSegmentationTester(img, detectorVec, finalBBoxVec);

        cv::Mat combined;
        cv::hconcat(img, imgWithBoundingBoxandSeg, combined);
        horizontalCombinedVector.push_back(combined);
    }

    cv::Mat combinedImage = pushOutTray(horizontalCombinedVector);

    cv::imwrite("Comple Tray.jpg", combinedImage);
    cv::imshow("Comple Tray", combinedImage);
    cv::waitKey(0);

    return 0;
}


// SPOSTARE QUANDO FINITO TUTTO
cv::Mat boundingBoxSegmentationTester(cv::Mat img, Detector detectorVec, std::vector<cv::Rect> finalBBoxVec) {

    cv::Mat clonedImgBBox = img.clone();
    cv::Mat clonedImgSeg = img.clone();
   
    for (int k = 0; k < detectorVec.getContours().size(); k++) {
        for (const auto& contour : detectorVec.getContours()) {

            cv::drawContours(clonedImgSeg, contour, -1, cv::Scalar(0, 255, 0), 5);
        }
    }

    for (const auto& rect : finalBBoxVec) {
        cv::rectangle(clonedImgBBox, rect, cv::Scalar(0, 0, 255), 10);
    }

    cv::Mat combined;
    cv::hconcat(clonedImgBBox, clonedImgSeg, combined);

    return combined;
}





