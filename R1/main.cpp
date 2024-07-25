#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

int main()
{
    cv::VideoCapture camera;
    camera.open(0);
    // cv::Mat img = cv::imread("./IMG.jpg");

    // cv::imshow("wow", img);
    // cv::waitKey(0);

    while (1)
    {

        cv::Mat image;
        camera >> image;

        imshow("window", image);

        int key = cv::waitKey(1);
        if (key == 'q')
            break;
    }

    return 0;
}