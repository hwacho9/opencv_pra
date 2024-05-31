#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;

int main()
{
    int thresh = 25;
    int max_diff = 10;
    int frame_count = 0;

    Mat a, b, c, draw;
    VideoCapture cap(0);
    cap.set(CAP_PROP_FRAME_WIDTH, 480);
    cap.set(CAP_PROP_FRAME_HEIGHT, 320);

    if (cap.isOpened())
    {
        cap >> a;
        cap >> b;

        while (true)
        {
            cap >> c;
            if (c.empty())
            {
                break;
            }
            draw = c.clone();

            Mat a_gray, b_gray, c_gray, diff1, diff2, diff1_t, diff2_t, diff;
            cvtColor(a, a_gray, COLOR_BGR2GRAY);
            cvtColor(b, b_gray, COLOR_BGR2GRAY);
            cvtColor(c, c_gray, COLOR_BGR2GRAY);

            absdiff(a_gray, b_gray, diff1);
            absdiff(b_gray, c_gray, diff2);

            threshold(diff1, diff1_t, thresh, 255, THRESH_BINARY);
            threshold(diff2, diff2_t, thresh, 255, THRESH_BINARY);

            bitwise_and(diff1_t, diff2_t, diff);

            Mat k = getStructuringElement(MORPH_CROSS, Size(3, 3));
            morphologyEx(diff, diff, MORPH_OPEN, k);

            int diff_cnt = countNonZero(diff);
            if (diff_cnt > max_diff)
            {
                std::vector<Point> nzero;
                findNonZero(diff, nzero);

                Rect bbox = boundingRect(nzero);
                rectangle(draw, bbox, Scalar(0, 255, 0), 2);

                putText(draw, "Motion detected!!", Point(10, 30),
                        FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 255));

                // std::string filename = "frame_" + std::to_string(frame_count) + ".jpg";
                // cv::imwrite(filename, draw);
                // frame_count++;
            }

            Mat diff_color;
            cvtColor(diff, diff_color, COLOR_GRAY2BGR);
            std::vector<Mat> imgs = {draw, diff_color};
            Mat stacked;
            hconcat(imgs, stacked);
            imshow("motion", stacked);

            a = b.clone();
            b = c.clone();

            if (waitKey(1) == 27)
            { // ESC key
                break;
            }
        }
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
