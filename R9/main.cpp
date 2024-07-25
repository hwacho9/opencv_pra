#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// Global variables
Mat templateImage;             // テンプレート画像
bool templateSelected = false; // テンプレートが選択されたかどうかのフラグ

// Mouse callback function to select template area
void onMouse(int event, int x, int y, int flags, void *userdata)
{
    static Point point1;
    if (event == EVENT_LBUTTONDOWN)
    {
        point1 = Point(x, y);
    }
    else if (event == EVENT_LBUTTONUP)
    {
        Point point2 = Point(x, y);
        Rect2d rect = Rect2d(point1, point2);
        templateImage = (*(Mat *)userdata)(rect).clone();
        templateSelected = true;
        imshow("Template", templateImage);
    }
}

int main()
{
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cout << "Error opening video stream" << endl;
        return -1;
    }

    namedWindow("Camera", 1);
    Mat frame;
    setMouseCallback("Camera", onMouse, &frame);

    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        if (templateSelected)
        {
            Mat result;
            matchTemplate(frame, templateImage, result, TM_CCOEFF_NORMED);
            double minVal, maxVal;
            Point minLoc, maxLoc;
            minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

            // TM_SQDIFF or TM_SQDIFF_NORMEDの場合は最小値を使用
            // それ以外の場合は最大値を使用
            Point matchLoc = maxLoc;
            rectangle(frame, matchLoc, Point(matchLoc.x + templateImage.cols, matchLoc.y + templateImage.rows), Scalar(0, 255, 0), 2, 8, 0);
        }

        imshow("Camera", frame);

        char key = (char)waitKey(30);
        if (key == 'q' || key == 27)
        {
            break; // Exit on ESC or 'q'
        }
    }

    cap.release();
    destroyAllWindows();
    return 0;
}