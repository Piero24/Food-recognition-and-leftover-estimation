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



cv::Mat boundingBoxTester(cv::Mat img, std::vector<cv::Vec3f> circlesVector, std::vector<cv::Rect> rectanglesVector);


int main(int argc, char** argv) {


    std::vector<cv::Mat> horizontalCombinedVector;
    std::vector<cv::Mat> trayVector = imgUploader(argc, argv);

    // VALIDO SOLO PER DEBUG
    // hcombinedVec = multipleTestPreProcessing(trayVector);
    // return 0;
    cv::Mat image0Preprocessed = segmentationPreprocessing(trayVector[0]);
    int numOfBoxes = findRectangularBoundingBoxes(trayVector[0], image0Preprocessed).size();
    
    size_t trayVectorSize = trayVector.size();
    for (size_t i = 3; i < 4; i++) {

        std::cout << "\n\n######################################## START IMAGE N: " << i + 1 << std::endl;
	
        cv::Mat img = trayVector[i].clone();

        //cv::Mat imagePreprocessed = testPreProcessing(img);
        cv::Mat imagePreprocessed = segmentationPreprocessing(img);
	//cv::imshow("Prep",imagePreprocessed);
        std::vector<cv::Vec3f> circlesVector = findCircularBoundingBoxes(img);
        std::vector<cv::Rect> rectanglesVector = findRectangularBoundingBoxes(img, imagePreprocessed);
        
        // TODO (MAYBE)
        cv::Mat thirdImgOut = thirdSegmentationFunc(img);

        // Ricrea la bounding box finale mergiando i metodi precedenti
        Detector detector;
        Detector detector1;
        detector1 = detector.subjectIsolator(img, circlesVector, rectanglesVector);
	
	std::vector<cv::Rect> rectsVector = detector1.getRectanglesVector();
	int currNumOfBoxes = rectsVector.size();
	cv::Mat noBackgroundImg;
	
        if(currNumOfBoxes>numOfBoxes)
	{
		int index;
        	int minArea = rectsVector.at(0).area();
		for(int j=0; j < rectsVector.size(); j++)
		{
			int area = rectsVector.at(j).area();
			
			if(area<minArea)
			{
				minArea = area;				
				index = j;
			}
		}
		// Ottenere l'iteratore corrispondente all'elemento da eliminare
		std::vector<cv::Rect>::iterator it = rectsVector.begin() + index;
		// Eliminare l'elemento utilizzando il metodo erase()
		rectsVector.erase(it);
		noBackgroundImg = img.clone();
		for (int k = 0; k < detector1.getContours().size(); k++)
		{
			for (const auto& contour : detector1.getContours())
			{
				cv::drawContours(noBackgroundImg, contour, -1, cv::Scalar(0, 255, 0), 2);
			}
		}
		for (const auto& rect : rectsVector)
		{
			cv::rectangle(noBackgroundImg, rect, cv::Scalar(0, 0, 255), 10);
		}
	}
	
	else{noBackgroundImg = detector1.getImage();}
	
        /*
        

                        NON MODIFICARE E CARICARE MAI IL MAIN SU GITHUB SENZA AVVISARE (POSSIBILMENTE EVITARE DI MODIFICARLO
                                                 -----


        */
        

        // TODO (QUANDO TUTTO QUELLO SOPRA E' FINITO):
        //
        //  - Segmentare cibo (se non gia fatto nella funzione precedente)
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
        cv::Mat imgWithBoundingBox = boundingBoxTester(img, circlesVector, rectanglesVector);


        cv::Mat combinedTest;
        cv::hconcat(img, imgWithBoundingBox, combinedTest);

        cv::Mat combined;
        cv::hconcat(combinedTest, noBackgroundImg, combined);
        horizontalCombinedVector.push_back(combined);

    }

    cv::Mat combinedImage = pushOutTray(horizontalCombinedVector);

    cv::imwrite("Comple Tray.jpg", combinedImage);
    cv::imshow("Comple Tray", combinedImage);
    cv::waitKey(0);

    return 0;
}


// SPOSTARE QUANDO FINITO TUTTO
cv::Mat boundingBoxTester(cv::Mat img, std::vector<cv::Vec3f> circlesVector, std::vector<cv::Rect> rectanglesVector){

    cv::Mat clonedImg = img.clone();

    for (const auto& circle : circlesVector) {
        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        cv::circle(clonedImg, center,  cvRound(circle[2]), cv::Scalar(0, 255, 255), 10);
    }

    for (const auto& rect : rectanglesVector) {
        rectangle(clonedImg, rect, cv::Scalar(0, 255, 0), 10);
    }

    return clonedImg;
}





