#include <opencv2/opencv.hpp>
using namespace cv;

int main()
{
    VideoCapture cap(0); // Open default camera
    if (!cap.isOpened())
        return -1;

    Mat frame, templateImage;
    bool templateSelected = false;

    namedWindow("Camera", 1);

    for (;;)
    {
        cap >> frame; // Capture frame from camera
        if (frame.empty())
            break;

        if (templateSelected)
        {
            double bestMatchScore = -1;
            Point bestMatchLoc;
            Size bestMatchSize;
            double bestMatchAngle;
            double bestMatchScale;

            for (double angle = 0; angle < 360; angle += 10)
            {
                Mat rotatedTemplate;
                Point2f center(templateImage.cols / 2.0F, templateImage.rows / 2.0F);
                Mat rotationMatrix = getRotationMatrix2D(center, angle, 1.0);
                warpAffine(templateImage, rotatedTemplate, rotationMatrix, templateImage.size());

                for (double scale = 0.5; scale <= 2.0; scale += 0.1)
                {
                    Mat scaledTemplate;
                    resize(rotatedTemplate, scaledTemplate, Size(), scale, scale, INTER_LINEAR);

                    if (scaledTemplate.cols > frame.cols || scaledTemplate.rows > frame.rows)
                        continue;

                    Mat result;
                    matchTemplate(frame, scaledTemplate, result, TM_CCOEFF_NORMED);
                    double minVal, maxVal;
                    Point minLoc, maxLoc;
                    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

                    if (maxVal > bestMatchScore)
                    {
                        bestMatchScore = maxVal;
                        bestMatchLoc = maxLoc;
                        bestMatchSize = scaledTemplate.size();
                        bestMatchAngle = angle;
                        bestMatchScale = scale;
                    }
                }
            }

            rectangle(frame, bestMatchLoc, Point(bestMatchLoc.x + bestMatchSize.width, bestMatchLoc.y + bestMatchSize.height), Scalar(0, 255, 0), 2, 8, 0);
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