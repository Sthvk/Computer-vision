#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


/////////////////  Draw Shapes and Text  //////////////////////

void main() {

    //Blank image
    Mat img(512, 512, CV_8UC3, Scalar(255, 255, 255));

    circle(img, Point(256, 256), 155, Scalar(0,69,256), FILLED);
    rectangle(img, Point(130, 226), Point(382, 286), Scalar(255, 255, 255), 3);

    putText(img, "roller skate", Point(137, 262), FONT_HERSHEY_COMPLEX,0.75, Scalar(0, 69, 255), 2);

    imshow("Image", img);

    waitKey(0);

}
