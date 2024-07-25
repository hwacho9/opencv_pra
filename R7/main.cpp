#include <opencv2/opencv.hpp>

using namespace cv;

int main()
{
    // (0) Create window and trackbar
    namedWindow("Edge Detection", WINDOW_AUTOSIZE);
    int thresholdValue = 100;
    createTrackbar("Threshold", "Edge Detection", &thresholdValue, 255);

    // (1) Define Sobel operators
    Mat sobelX = (Mat_<double>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
    Mat sobelY = (Mat_<double>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);

    VideoCapture cap(0); // Capture from default camera

    Mat frame, gray, gradX, gradY, absGradX, absGradY, grad, edgeDetected;

    while (true)
    {
        // (2) Capture a new image
        cap >> frame;
        if (frame.empty())
            break;

        // (3) Convert to grayscale
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // (4) Apply Sobel operators
        filter2D(gray, gradX, CV_16S, sobelX);
        filter2D(gray, gradY, CV_16S, sobelY);

        // (5) Calculate absolute values
        convertScaleAbs(gradX, absGradX);
        convertScaleAbs(gradY, absGradY);

        // (6) Add gradients
        addWeighted(absGradX, 0.5, absGradY, 0.5, 0, grad);

        // (7) Apply threshold
        threshold(grad, edgeDetected, thresholdValue, 255, THRESH_BINARY);

        // (8) Convert to 8-bit image
        edgeDetected.convertTo(edgeDetected, CV_8U);

        // (9) Display result
        imshow("Edge Detection", edgeDetected);

        // Exit on ESC key press
        if (waitKey(30) == 27)
            break;
    }

    // Release resources
    cap.release();
    destroyAllWindows();

    return 0;
}