#include <opencv2/opencv.hpp>
#include <iostream>


using namespace cv;
using namespace std;

Mat im, imCanny, display;

int thresh, maxThreshold = 3 * 255;

RNG rng(12345);

void callback(int, void *)
{
    vector<vector<Point> >contours;
    vector<Vec4i>hierarchy;

    Canny(im, imCanny, thresh, thresh*2, 3);

    findContours(imCanny, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    if(display.empty())
        display = Mat::zeros(imCanny.size(), CV_8UC3);
    else
        display.setTo(Scalar(0, 0, 0));

    for(size_t i=0; i<contours.size();i++)
    {
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        drawContours(display, contours, (int)i, color, 2);
    }

    imshow("Contours", display);
}

int main(int argc, char** argv)
{
    string filename("./images/sample.png");
    if(argc>1)
        filename = argv[1];

    im = imread(filename, IMREAD_GRAYSCALE);

    namedWindow("Contours", WINDOW_AUTOSIZE);
    imshow("Contours", im);

    createTrackbar(" Canny thresh:", "Contours", &thresh, maxThreshold, callback);
    callback(0, 0);

    waitKey(0);
    return 0;
}






