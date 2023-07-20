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


void leftFood(std::vector<cv::Mat>& imgM, std::vector<std::string>& imageNames,std::vector<double>& diff)
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
    std::vector<std::string> sortedImageNames(imageNames.size()); 
    std::vector<cv::Mat> sortedimgM(imgM.size()); 
    for (size_t i = 0; i < indices.size(); i++) { 
        sortedImageNames[i] = imageNames[indices[i]]; 
        sortedimgM[i] = imgM[indices[i]]; 
    }
    
    imgM=sortedimgM;
    imageNames=sortedImageNames;   
    std::string currentName = imageNames[0];
     cv::Mat currentImage = imgM[0]; 
    for (size_t i = 0; i < imageNames.size()-1; i++) { 
    std::string nextName = imageNames[i+1]; 
    cv::Mat nextImage = imgM[i+1]; 
 
    if(currentName!=nextName)
    {
    	currentName=nextName;
    	currentImage=nextImage;
    }
    else{
    if(currentName==nextName)
    {  
        cv::Mat alpha; 
    	cv::extractChannel(currentImage, alpha, 3); 
 
    	// Crea una maschera per selezionare solo i pixel con alpha diverso da 0 (non trasparenti) 
    	cv::Mat mask; 
    	cv::compare(alpha, 0, mask, cv::CMP_GT); 
 
    	// Conta i pixel non trasparenti 
    	int count = cv::countNonZero(mask);
        
        
        cv::Mat alpha1; 
    	cv::extractChannel(nextImage, alpha1, 3); 
 
    	// Crea una maschera per selezionare solo i pixel con alpha diverso da 0 (non trasparenti) 
    	cv::Mat mask1; 
    	cv::compare(alpha1, 0, mask1, cv::CMP_GT); 

            double diff1 = (static_cast<double>(cv::countNonZero(mask1)) / static_cast<double>(cv::countNonZero(mask)))*100;
            if(diff1>100)
            diff1=100;
            diff.push_back(diff1);
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
    sift->detectAndCompute(img1, cv::noArray(), kp1, des1); 
    sift->detectAndCompute(img2, cv::noArray(), kp2, des2); 

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

