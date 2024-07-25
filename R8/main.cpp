#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// Global variables for mouse callback
Point point1, point2;         // Points to define the rectangle (top-left, bottom-right)
bool drawing = false;         // Flag to indicate drawing in progress
bool rectangle_drawn = false; // Flag to indicate if the rectangle has been drawn

// Mouse callback function
void onMouse(int event, int x, int y, int flags, void *userdata)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        // When left mouse button is pressed, start drawing
        drawing = true;
        point1 = Point(x, y);
        point2 = point1; // Initialize point2
    }
    else if (event == EVENT_MOUSEMOVE && drawing)
    {
        // Update the bottom-right point of the rectangle as the mouse moves
        point2 = Point(x, y);
    }
    else if (event == EVENT_LBUTTONUP)
    {
        // Finish drawing the rectangle
        drawing = false;
        rectangle_drawn = true;
    }
}

int main()
{
    // Open the default camera
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cout << "Error opening video stream" << endl;
        return -1;
    }

    // Create a window
    namedWindow("Camera", 1);
    // Set the mouse callback function for the window
    setMouseCallback("Camera", onMouse, NULL);

    Mat frame, frameCopy;
    while (true)
    {
        cap >> frame; // Capture a new frame
        if (frame.empty())
            break;

        frame.copyTo(frameCopy); // Make a copy of the frame

        if (rectangle_drawn)
        {
            // Draw the rectangle on the copy of the frame
            rectangle(frameCopy, point1, point2, Scalar(0, 255, 0), 2);
        }

        imshow("Camera", frameCopy); // Show the frame

        char key = (char)waitKey(30);
        if (key == 'q' || key == 27)
        {
            break; // Exit on ESC or 'q'
        }
        else // Inside the main loop, where 'c' is pressed and a rectangle has been drawn
            if (key == 'c' && rectangle_drawn)
            {
                // Ensure the rectangle has positive width and height
                if ((point2.x - point1.x) > 0 && (point2.y - point1.y) > 0)
                {
                    Rect roi(point1, point2);
                    // Check if the ROI is within the frame bounds
                    if (roi.x >= 0 && roi.y >= 0 && roi.width + roi.x <= frame.cols && roi.height + roi.y <= frame.rows)
                    {
                        Mat croppedImage = frame(roi);
                        imshow("Cropped", croppedImage); // Show the cropped image in a new window
                    }
                    else
                    {
                        cout << "ROI is out of bounds." << endl;
                    }
                }
                else
                {
                    cout << "Invalid rectangle dimensions." << endl;
                }
            }
    }

    cap.release();
    destroyAllWindows();
    return 0;
}