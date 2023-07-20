#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"

#include <opencv2/features2d.hpp>

#include "../include/matchImage.h"



std::vector<cv::Mat> loadImage(std::string pathPatch, std::vector<std::string>& imageNames){

// Percorso della cartella contenente le immagini 
    std::string folderPath = pathPatch + "/imageBase/"; 
    //nome
    //std::vector<std::string> imageNames;
    // Vettore per memorizzare le immagini 
    std::vector<cv::Mat> images;

    std::vector<cv::String> fileInFolderVec;
    cv::glob(folderPath, fileInFolderVec, false);
    
    size_t count = fileInFolderVec.size();
    for (size_t i = 0; i < count; i++) {
        // Verifica se il file è un'immagine 
        if (fileInFolderVec[i].substr(fileInFolderVec[i].size() - 4) == ".jpg" ||
            fileInFolderVec[i].substr(fileInFolderVec[i].size() - 4) == ".JPG" ||
            fileInFolderVec[i].substr(fileInFolderVec[i].size() - 4) == ".png" ||
            fileInFolderVec[i].substr(fileInFolderVec[i].size() - 4) == ".bmp") { 
            // Carica l'immagine e la aggiunge al vettore 
            std::string imageName = fileInFolderVec[i];
            cv::Mat image = cv::imread(imageName); 
            if (!image.empty()) { 
                images.push_back(image);
                imageName.erase(imageName.size()-4);
                if(isdigit(imageName.back()))
                imageName.erase(imageName.size()-1);
                imageName.erase(0,21);
                imageNames.push_back(imageName);
            } 
        } 
    }
    
    return images;
}

/*
void leftFood(std::vector<cv::Mat> imgM, std::vector<cv::Mat>& imgL, std::vector<std::string>& imageNames, int n)
{

    std::vector<size_t> indices(imageNames.size()); 
    for (size_t i = 0; i < indices.size(); i++) { 
        indices[i] = i; 
    } 
 
    // Ordina l'array1 in base all'ordine alfabetico utilizzando l'array di indici 
    std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b) { 
        return imageNames[a] < imageNames[b]; 
    }); 
 
    // Usa l'array di indici ordinato per riordinare anche array2 
    std::vector<std::string> sortedArray1(imageNames.size()); 
    std::vector<cv::Mat> sortedArray2(imgM.size()); 
    for (size_t i = 0; i < indices.size(); i++) { 
        sortedArray1[i] = imageNames[indices[i]]; 
        sortedArray2[i] = imgM[indices[i]]; 
    }
    

    std::copy(sortedArray2.begin(),sortedArray2.end(),imgM.begin());
    std::copy(sortedArray1.begin(),sortedArray1.end(),imageNames.begin());
    
    
    for (size_t i = 0; i < imageNames.size(); i++) { 
    std::string currentName = imageNames[i]; 
    cv::Mat currentImage = imgM[i]; 
 
    // Controllo se l'elemento corrente è uguale all'elemento precedente 
    if (i > 0 && currentName == imageNames[i - 1]) { 
        continue; // Salta l'iterazione se l'elemento è uguale al precedente 
    } 
 
    // Ciclo for nidificato per confrontare l'elemento corrente con tutti gli altri dello stesso tipo 
    for (size_t j = i + 1; j < imageNames.size(); j++) { 
        if (currentName == imageNames[j]) { 
        cv::Mat greyImage1,greyImage2;
        cv::cvtColor(currentImage, greyImage1, cv::COLOR_BGR2GRAY);
        cv::cvtColor(imgM[j], greyImage2, cv::COLOR_BGR2GRAY);
            int diff = cv::countNonZero(greyImage2) / cv::countNonZero(greyImage1);

            std::cout << "Divido l'immagine " << imageNames[j] << " con l'immagine " << currentName << std::endl;
            std::cout << "Differenza pixel " << diff << std::endl;
 

        } 
    } 
}
    
*/

void leftFood(std::vector<cv::Mat> imgM, std::vector<cv::Mat>& imgL, std::vector<std::string>& imageNames, int n) 
{ 
    std::vector<size_t> indices(imageNames.size()); 
    for (size_t i = 0; i < indices.size(); i++) { 
        indices[i] = i; 
    } 
 
    // Ordina l'array1 in base all'ordine alfabetico utilizzando l'array di indici 
    std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b) { 
        return imageNames[a] < imageNames[b]; 
    }); 
 
    // Usa l'array di indici ordinato per riordinare anche array2 
    std::vector<std::string> sortedArray1(imageNames.size()); 
    std::vector<cv::Mat> sortedArray2(imgM.size()); 
    for (size_t i = 0; i < indices.size(); i++) { 
        sortedArray1[i] = imageNames[indices[i]]; 
        sortedArray2[i] = imgM[indices[i]]; 
    } 
 
    // Copia gli array ordinati nell'originale 
    imgM = sortedArray2; 
    imageNames = sortedArray1; 
 
    for (size_t i = 0; i < imageNames.size(); i++) { 
        std::string currentName = imageNames[i]; 
        cv::Mat currentImage = imgM[i]; 
 
        // Controllo se l'elemento corrente è uguale all'elemento precedente 
        if (i > 0 && currentName == imageNames[i - 1]) { 
            continue; // Salta l'iterazione se l'elemento è uguale al precedente 
        } 
 
        // Ciclo for nidificato per confrontare l'elemento corrente con tutti gli altri dello stesso tipo 
        for (size_t j = i + 1; j < imageNames.size(); j++) { 
            if (currentName == imageNames[j]) { 
                cv::Mat greyImage1, greyImage2; 
                cv::cvtColor(currentImage, greyImage1, cv::COLOR_BGR2GRAY); 
                cv::cvtColor(imgM[j], greyImage2, cv::COLOR_BGR2GRAY); 
 
                int diff = cv::countNonZero(greyImage2) / cv::countNonZero(greyImage1); 
                std::cout << "Divido l'immagine " << imageNames[j] << " con l'immagine " << currentName << std::endl; 
                std::cout << "Differenza pixel " << diff << std::endl; 
            } 
        } 
    } 
}


void imgMatching(std::vector<cv::Mat> imgM, std::vector<cv::Mat>& imgL, std::vector<std::string>& imageNames, int n) {

   std::vector<cv::Mat> imgMReturn;
   std::vector<std::string> imageNamesReturn;
   cv::Mat img1,img2;
   double v=0; int x=0;
   int m=n-1;
   
   //imgM contiene le immagini tagliate
   //imageNames contiente i nomi tutti all'inizio tutti quelli caricati, poi resetta e aggiunge solo quelli di interesse(n)
   //ImgL le immagini tuttiutti all'inizio tutti quelli caricati, poi resetta e aggiunge solo quelli di interesse(n)

   for(int k=0; k<imgM.size();k++)
   {
    img1=imgM[k];
    v=0; x=0;
    
    for(size_t i=0; i< imgL.size(); i++)
    {
     
    img2=imgL[i];
    // Initiate SIFT detector 
    cv::Ptr<cv::SIFT> sift = cv::SIFT::create();

    // find the keypoints and descriptors with SIFT 
    std::vector<cv::KeyPoint> kp1, kp2; 
    cv::Mat des1, des2;

    sift -> detectAndCompute(img1, cv::noArray(), kp1, des1); 
    sift -> detectAndCompute(img2, cv::noArray(), kp2, des2); 

    // FLANN parameters 
    cv::Ptr<cv::FlannBasedMatcher> flann = cv::FlannBasedMatcher::create(); 
    std::vector<std::vector<cv::DMatch>> matches; 
    flann->knnMatch(des1, des2, matches, 2); 
 
    // Need to draw only good matches, so create a mask 
    std::vector<cv::DMatch> goodMatches; 
    for (size_t t = 0; t < matches.size(); t++) { 
        if (matches[t][0].distance < 0.67 * matches[t][1].distance) { //ottimale tra 0,67 e 0,7
            goodMatches.push_back(matches[t][0]); 
        } 
    }

    if (goodMatches.size() >v){

		v=goodMatches.size();
		x=i;
    }

    if(i+1== imgL.size()){
	if(m>0){
		imageNamesReturn.push_back(imageNames[x]);
		imgMReturn.push_back(imgL[x]);
		m--;
		} else if(m==0) {
	
				imageNamesReturn.push_back(imageNames[x]);
				imgMReturn.push_back(imgL[x]);
				
				imgL=imgMReturn;
				imageNames=imageNamesReturn;
				/*
				imgL.clear();
				imgL.resize(imgMReturn.size());//qua è 2
				std::copy(imgMReturn.begin(),imgMReturn.end(),imgL.begin());

				imageNames.clear();
				imageNames.resize(imageNamesReturn.size());
				std::copy(imageNamesReturn.begin(),imageNamesReturn.end(),imageNames.begin());
				*/
    				m--;
    			
    				} else {
    			imageNames.push_back(imageNames[x]);
		}
    	}
    	
    }
    	
   }


}

