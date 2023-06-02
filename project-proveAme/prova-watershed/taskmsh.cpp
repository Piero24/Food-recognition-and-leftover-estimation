
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
    cv::pyrMeanShiftFiltering(src,src,10,20);
    
    //GaussianBlur(src, src, cv::Size(3,3),0);
    //src.convertTo(src, -1, 1.0, 8.0);

    imshow("Final Result", src);
    waitKey();
    return 0;
}

