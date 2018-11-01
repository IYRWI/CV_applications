#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;

Mat src, edges;

int lowThreshold = 50;
int highThreshold = 100;

const int maxThreshold = 1000;

int apertureSizes[] = {3, 5, 7};
int maxapertureIndex = 2;
int apertureIndex = 0;

int blurAmount = 0;
int maxBlurAmount = 20;

void applyCanny(int ,void *)
{
    Mat blurredSrc;

    if(blurAmount > 0)
        GaussianBlur(src, blurredSrc, Size(2 * blurAmount + 1, 2 * blurAmount + 1), 0);
    else
        blurredSrc = src.clone();

    int apertureSize = apertureSizes[apertureIndex];

    Canny( blurredSrc, edges, lowThreshold, highThreshold, apertureSize);

    imshow("Edges", edges);
}

int main(void)
{
    src = imread("./images/sample.png", IMREAD_GRAYSCALE)Im
    imshow("Edges", src);
    namedWindow("Edges", CV_WINDOW_AUTOSIZE);

    createTrackbar("Low Threshold", "Edges", &lowThreshold, maxThreshold, applyCanny);

    createTrackbar("High Threshold", "Edges", &highThreshold, maxThreshold, applyCanny);

    createTrackbar("aperture Size", "Edges", &apertureIndex, maxapertureIndex, applyCanny);

    createTrackbar("Blur", "Edges", &blurAmount, maxBlurAmount, applyCanny);

    waitKey(0);
    destroyAllWindows();
}



