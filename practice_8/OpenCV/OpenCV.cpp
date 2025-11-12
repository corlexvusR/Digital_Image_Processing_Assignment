#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>
#include <iostream>

using namespace cv;

int main()
{
    Mat image = imread("test.png", IMREAD_COLOR);

    imshow("Window", image);

    waitKey();

    return 0;
}