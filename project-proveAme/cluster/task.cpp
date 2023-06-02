
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main(int argc, char *argv[])
{

    string i1=argv[1];
    if((i1=="food_image.jpg"||i1=="leftover1.jpg"||i1=="leftover2.jpg"||i1=="leftover3.jpg"))
    {
    	cout <<" Nice image man!\n";
    }
    else
    {
    	cout <<"Wrong image man!\n";
    	return 0;
    }
    Mat src = imread(i1);
    

    src.convertTo(src, -1, 1.0, 40);
    //imshow("Source Image", src);
    //GaussianBlur(src, src, cv::Size(3,3),0);
    cv::pyrMeanShiftFiltering(src,src,10,20); 

    //GaussianBlur( src, src, cv::Size(3,3), 0, 0 );
    Mat p = Mat::zeros(src.cols*src.rows, 5, CV_32F);
    Mat bestLabels, centers, clustered;
    vector<Mat> bgr;
    cv::split(src, bgr);
    for(int i=0; i<src.cols*src.rows; i++) {
        p.at<float>(i,0) = (i/src.cols) / src.rows;
        p.at<float>(i,1) = (i%src.cols) / src.cols;
        p.at<float>(i,2) = bgr[0].data[i] / 255.0;
        p.at<float>(i,3) = bgr[1].data[i] / 255.0;
        p.at<float>(i,4) = bgr[2].data[i] / 255.0;
    }

    int K = 4;
    cv::kmeans(p, K, bestLabels,
            TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 10, 1.0),
            3, KMEANS_PP_CENTERS, centers);

    int colors[K];
    for(int i=0; i<K; i++) {
        colors[i] = 255/(i+1);
    }
    clustered = Mat(src.rows, src.cols, CV_32F);
    for(int i=0; i<src.cols*src.rows; i++) {
        clustered.at<float>(i/src.cols, i%src.cols) = (float)(colors[bestLabels.at<int>(0,i)]);
    }

    clustered.convertTo(clustered, CV_8U);
    imshow("clustered", clustered);
    waitKey();
    return 0;
}

