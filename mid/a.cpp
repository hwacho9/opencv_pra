#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    int thresh = 25;
    int max_diff = 5;

    cv::Mat a, b, a_gray, b_gray, diff1, diff_t, diff;
    cv::VideoCapture cap(0);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 600);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 400);

    if (cap.isOpened())
    {
        cap >> a;

        while (true)
        {
            cap >> b;
            if (b.empty())
            {
                break;
            }

            cv::cvtColor(a, a_gray, cv::COLOR_BGR2GRAY);
            cv::cvtColor(b, b_gray, cv::COLOR_BGR2GRAY);

            cv::absdiff(a_gray, b_gray, diff1);
            cv::threshold(diff1, diff_t, thresh, 255, cv::THRESH_BINARY);

            cv::Mat k = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
            cv::morphologyEx(diff_t, diff, cv::MORPH_OPEN, k);

            int diff_cnt = cv::countNonZero(diff);
            if (diff_cnt > max_diff)
            {
                std::vector<cv::Point> nzero;
                cv::findNonZero(diff, nzero);

                cv::Rect bbox = cv::boundingRect(nzero);
                cv::rectangle(b, bbox, cv::Scalar(0, 255, 0), 2);

                cv::putText(b, "Motion detected!!", cv::Point(10, 30),
                            cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(0, 0, 255));
            }

            cv::imshow("motion", b);

            a = b.clone();

            if (cv::waitKey(1) == 27)
            { // ESC key
                break;
            }
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
