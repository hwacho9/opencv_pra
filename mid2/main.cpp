#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat src, templateImage;
bool templateSelected = false;
double bestMatchScore = -1;
Point bestMatchLoc;
Size bestMatchSize;
double bestMatchAngle;
double bestMatchScale;

// Mouse callback for selecting the template region
void onMouse(int event, int x, int y, int flags, void *userdata)
{
    static Point startPoint;
    if (event == EVENT_LBUTTONDOWN)
    {
        startPoint = Point(x, y);
    }
    else if (event == EVENT_LBUTTONUP)
    {
        Point endPoint = Point(x, y);
        Rect2d selectedRegion = Rect2d(startPoint, endPoint);
        templateImage = (*(Mat *)userdata)(selectedRegion).clone();
        templateSelected = true;
        imshow("Template", templateImage);
    }
}

// Function to perform template matching with rotation and scaling
void performTemplateMatching(const Mat &src, const Mat &templateImage)
{
    double bestMatchScore = -1;
    Point bestMatchLoc;
    double bestMatchAngle = 0;
    double bestMatchScale = 1;

    // 이미지에 모든 매칭 결과와 최고의 매칭 결과 표시
    Mat displayImage = src.clone(); // 결과를 표시할 이미지 복사본

    // 임계값 설정
    const double threshold = 0.68; // 유사도 임계값, 필요에 따라 조정

    for (double angle = 0; angle < 360; angle += 10)
    {
        Mat rotatedTemplate;
        Point2f center(templateImage.cols / 2.0, templateImage.rows / 2.0);
        Mat rotationMatrix = getRotationMatrix2D(center, angle, 1.0);
        warpAffine(templateImage, rotatedTemplate, rotationMatrix, templateImage.size());

        for (double scale = 0.5; scale <= 2.0; scale += 0.1)
        {
            Mat scaledTemplate;
            resize(rotatedTemplate, scaledTemplate, Size(), scale, scale, INTER_LINEAR);

            if (scaledTemplate.cols > src.cols || scaledTemplate.rows > src.rows)
                continue;

            Mat result;
            matchTemplate(src, scaledTemplate, result, TM_CCOEFF_NORMED);

            // 모든 위치에 대해 유사도가 임계값 이상인지 확인하고 표시
            double minVal;
            double maxVal;
            Point minLoc;
            Point maxLoc;
            Point matchLoc;

            for (int y = 0; y < result.rows; y++)
            {
                for (int x = 0; x < result.cols; x++)
                {
                    double score = result.at<float>(y, x);
                    if (score > threshold)
                    {
                        // 유사도가 임계값 이상인 경우 로그 출력
                        std::cout << "Match found at (" << x << ", " << y << ") with score: " << score
                                  << ", Angle: " << angle << " degrees, Scale: " << scale << std::endl;

                        // 매칭된 영역에 사각형 그리기
                        rectangle(displayImage, Point(x, y), Point(x + scaledTemplate.cols, y + scaledTemplate.rows), Scalar(0, 255, 0), 2);

                        // 매칭 정보 표시 (선택적)
                        String info = format("Score: %.2f, Angle: %.0f, Scale: %.2f", score, angle, scale);
                        putText(displayImage, info, Point(x, y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 255), 1);
                    }
                }
            }

            // 최고의 매칭 결과 찾기
            minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
            if (maxVal > bestMatchScore)
            {
                bestMatchScore = maxVal;
                bestMatchLoc = maxLoc;
                bestMatchAngle = angle;
                bestMatchScale = scale;
            }
        }
    }

    // 최고의 매칭 결과에 특별한 표시
    rectangle(displayImage, bestMatchLoc, Point(bestMatchLoc.x + templateImage.cols * bestMatchScale, bestMatchLoc.y + templateImage.rows * bestMatchScale), Scalar(0, 0, 255), 2);
    String bestMatchInfo = format("Best Match: Score: %.2f, Angle: %.0f", bestMatchScore, bestMatchAngle);
    putText(displayImage, bestMatchInfo, Point(bestMatchLoc.x, bestMatchLoc.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);

    // 결과 이미지 표시
    namedWindow("Matching Results", WINDOW_AUTOSIZE);
    imshow("Matching Results", displayImage);
    waitKey(0);
}

int main()
{
    src = imread("/Users/chosunghwa/Desktop/workspace/opencv_pra/mid2/6.jpg");
    if (src.empty())
    {
        cout << "Error loading the image" << endl;
        return -1;
    }

    namedWindow("Image", WINDOW_AUTOSIZE);
    imshow("Image", src);

    setMouseCallback("Image", onMouse, (void *)&src);
    waitKey(0); // Wait for the user to select a template

    if (templateSelected)
    {
        performTemplateMatching(src, templateImage);
        namedWindow("Matched Image", WINDOW_AUTOSIZE);
        imshow("Matched Image", src);
        waitKey(0);
    }
    else
    {
        cout << "Template not selected." << endl;
    }

    return 0;
}