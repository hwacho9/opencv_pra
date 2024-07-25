#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(void)
{

    VideoCapture camera;
    camera.open(1);

    int hueMin = 0;
    int hueMax = 181;
    int satMin = 0;
    int satMax = 255;
    int valMin = 0;
    int valMax = 255;

    namedWindow("Slider");
    resizeWindow("Slider", 600, 400);

    createTrackbar("Hue Min", "Slider", &hueMin, 181); // �ŏ��l
    createTrackbar("Hue Max", "Slider", &hueMax, 181);
    createTrackbar("Sat Min", "Slider", &satMin, 256);
    createTrackbar("Sat Max", "Slider", &satMax, 256);
    createTrackbar("Val Min", "Slider", &valMin, 256);
    createTrackbar("Val Max", "Slider", &valMax, 256);

    while (1)
    {

        Mat rgbImage;
        camera >> rgbImage;

        Mat hsvImage;
        cvtColor(rgbImage, hsvImage, COLOR_BGR2HSV);
        Mat maskImage;
        inRange(hsvImage, Scalar(hueMin, satMin, valMin), Scalar(hueMax, satMax, valMax), maskImage);

        Mat resultImage(rgbImage.cols, rgbImage.rows, CV_8UC3); //
        resultImage.setTo(Scalar(0, 0, 0));
        rgbImage.copyTo(resultImage, maskImage); // rgbImage��mask Image�𕡐�����resultImage�Ƃ��Ă���

        int col = rgbImage.cols;
        int row = rgbImage.rows; // size �擾

        Mat backgroundImage;

        int bg_col = backgroundImage.cols;
        int bg_row = backgroundImage.rows;

        Mat backgroundImageOutput;
        backgroundImage = imread("heaven.jpg");                                                 // background�摜�ǂݍ���
        resize(backgroundImage, backgroundImageOutput, cv::Size(col, row), 0, 0, INTER_LINEAR); // resize
        imshow("window3", backgroundImageOutput);                                               // resize���������炵���vimg
        // imshow("window2", backgroundImage); // �V����window�ŏo��

        // �����Ă����ʐ^���}�X�N�̍����Ƃ���ɑΉ����ăR�s�[����

        // rgbimage ��background ��mask�����ҁ[
        rgbImage.copyTo(backgroundImageOutput, maskImage);

        imshow("RGB Image", rgbImage);
        imshow(" Mask Image", maskImage);
        imshow("Result Image", resultImage);
        imshow("croma Image", backgroundImageOutput);

        int key = waitKey(1);
        if (key == 'q')
            break;
    }

    return 0;
}
