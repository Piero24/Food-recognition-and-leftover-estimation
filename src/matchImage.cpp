#include <iostream>
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ximgproc/segmentation.hpp"



#include <opencv2/features2d.hpp>
#include <filesystem> 


#include "../include/matchImage.h"



std::vector<cv::Mat> loadImage(std::vector<std::string>& imageNames){

// Percorso della cartella contenente le immagini 
    std::string folderPath = "../dataset/imageBase"; 
    //nome
    //std::vector<std::string> imageNames;
    // Vettore per memorizzare le immagini 
    std::vector<cv::Mat> images; 
    
       // Ciclo attraverso tutti i file nella cartella 
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) { 
        // Verifica se il file è un'immagine 
        if (entry.is_regular_file() && entry.path().extension() == ".jpg") { 
            // Carica l'immagine e la aggiunge al vettore 
            std::string imageName= entry.path().string();
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
	
				imgL.clear();
				imgL.resize(imgMReturn.size());//qua è 2
				std::copy(imgMReturn.begin(),imgMReturn.end(),imgL.begin());

				imageNames.clear();
				imageNames.resize(imageNamesReturn.size());
				std::copy(imageNamesReturn.begin(),imageNamesReturn.end(),imageNames.begin());
    				m--;
    			
    				} else {
    			imageNames.push_back(imageNames[x]);
		}
    	}
    	
    }
    	
   }


}
