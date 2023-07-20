// clear && make && ./Food-Recognition ./Food-recognition-and-leftover-estimation/dataset

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

#include "../include/matchImage.h"

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

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_string>" << std::endl;
        return 1;
    }

    for (int trayNumber = 1; trayNumber < 9; trayNumber++) {

        //Parte Ame
        //imgL contiene le immagini totali che dopo andranno tolte
        //imageNames contiente i nomi
        //imgM contiene le immagini ritagliate al momento da contour
        //n parametro per sapere quante sono le classi iniziali
        
        std::vector<cv::Mat> imgM;
        std::vector<std::string> imageNames;
        std::vector<cv::Mat> imgL = loadImage(imageNames);
        int n=0;
        //fine parte Ame


        std::vector<cv::Mat> horizontalCombinedVector;
        std::string trayPath = argv[1];
        trayPath += "/tray" + std::to_string(trayNumber) + "/";

        std::cout << "\n\n######################################## START TRAY N: " << trayNumber << " ########################################" << std::endl;

        std::vector<cv::Mat> trayVector = imgUploader(trayPath);

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
            // cv::Mat thirdImgOut = thirdSegmentationFunc(img);

            // Ricrea la bounding box finale mergiando i metodi precedenti
            Detector detector;
            Detector detectorVec;
            detectorVec = detector.subjectIsolator(img, circlesVector, rectanglesVector);
            std::vector<cv::Rect> finalBBoxVec = detector.fromSegmentationToBBox(img, detectorVec, numOfBoxes);

            //parte Ame
            cv::Mat img1;
            for (const auto& contour : detectorVec.getContours())
            {
                img1=img.clone();
                cv::drawContours(img1, contour, -1, cv::Scalar(0, 0, 0), 2);
                cv::Mat imgMask = cv::Mat::zeros(img.size(), CV_8UC1);
                cv::fillPoly(imgMask, contour, cv::Scalar(255,255,255));
                cv::Mat imgFull = cv::Mat::zeros(img.size(), CV_8UC1);
                cv::bitwise_and(img, img, imgFull, imgMask);
                imgM.push_back(imgFull);
                if(i==0)
                {n++;}
            }
            //fine parte Ame
        

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

        //parte Ame
        //in imgM ci saranno le immagini tagliate, 
        //imgL solo le immagini matchate al primo giro
        //in imageNames i nomi rispetto alle imgM
        imgMatching(imgM,imgL,imageNames,n);
        
        
        /* se volete provare
        for (const auto& a : imgM)
            {

                cv::imshow("A", a);
                cv::waitKey();
            
            }	
            for (const auto& a : imgL)
            {
                //cout << "Nome: " << imageNames[x] << endl;
                cv::imshow("A", a);
                cv::waitKey();
            
            }
            
            for (const auto& a : imageNames)
            {
                std::cout << "Nome: " << a << std::endl;
            
            }
        */
        
        //fine parte Ame

        cv::Mat combinedImage = pushOutTray(horizontalCombinedVector);

        std::string trayName = "Complete Tray " + std::to_string(trayNumber);
        std::string trayNameForSave = trayName + ".jpg";

        cv::imwrite(trayNameForSave, combinedImage);
        cv::imshow(trayName, combinedImage);
        //cv::waitKey(0);

    }

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

    std::cout << "\n\n############## CORNER OF THE BOUNDING BOX" << std::endl;
    
    int i = 0;
    for (const auto& rect : finalBBoxVec) {
        cv::rectangle(clonedImgBBox, rect, cv::Scalar(0, 0, 255), 10);
        std::cout << "ID: " << i << "; [" << rect.x << ", " << rect.y << ", "  << rect.x + rect.width << ", "  << rect.y + rect.height << "]" << std::endl;
        i++;
    }

    cv::Mat combined;
    cv::hconcat(clonedImgBBox, clonedImgSeg, combined);

    return combined;
}
