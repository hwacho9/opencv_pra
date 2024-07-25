
#include <opencv2/opencv.hpp>
using namespace cv;

int main(void)
{

    VideoCapture camera;
    camera.open(0);

    int threshold_value = 127;

    namedWindow("Slider");
    resizeWindow("Slider", 600, 50);
    createTrackbar("Threshold", "Slider", &threshold_value, 256);

    while (1)
    {

        Mat rgbImage;
        camera >> rgbImage;
        imshow("RGB Image", rgbImage);

        Mat grayImage;
        cvtColor(rgbImage, grayImage, COLOR_BGR2GRAY);
        imshow("Gray Image", grayImage);

        static Mat background;
        Mat diffImage;

        if (!background.empty())
        {

            absdiff(grayImage, background, diffImage);
            imshow("Difference", diffImage);

            Mat maskImage;
            threshold(diffImage, maskImage, (double)threshold_value, 255, THRESH_BINARY);
            imshow("Mask", maskImage);

            Mat resultImage(rgbImage.rows, rgbImage.cols, CV_8UC3);
            resultImage.setTo(Scalar(0, 0, 0));
            rgbImage.copyTo(resultImage, maskImage);
            imshow("Result Image", resultImage);
        }

        int key = waitKey(1);

        if (key == 'q')
            break;

        else if (key == 's')
        {

            grayImage.copyTo(background);
            imshow("Background", background);
        }
    }

    return 0;
}
