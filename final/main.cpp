#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cout << "Usage: ./ball_tracking <video_file>" << endl;
        return -1;
    }

    // ビデオファイルを開く
    VideoCapture cap(argv[1]);
    if (!cap.isOpened())
    {
        cout << "ビデオストリームまたはファイルを開く際のエラー: " << argv[1] << endl;
        return -1;
    }

    int thresh = 25;   // 差分を検出するための閾値
    int max_diff = 10; // 動きとして考慮される最大差分

    Mat a, b, c;                         // フレームを格納するためのMatオブジェクト
    cap.set(CAP_PROP_FRAME_WIDTH, 480);  // フレーム幅を設定
    cap.set(CAP_PROP_FRAME_HEIGHT, 320); // フレーム高さを設定

    // フレームを初期化
    cap >> a;
    cap >> b;

    while (true)
    {
        // 新しいフレームをキャプチャ
        cap >> c;
        if (c.empty())
            break;

        Mat draw = c.clone();

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

        vector<vector<Point>> contours;
        findContours(diff, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        for (const auto &contour : contours)
        {
            // 外接矩形を計算
            Rect boundingBox = boundingRect(contour);

            // サイズフィルタリング (小さすぎるまたは大きすぎる領域を除外)
            if (boundingBox.area() > 500 && boundingBox.area() < 5000)
            {
                // 円形性の検出
                double aspectRatio = (double)boundingBox.width / boundingBox.height;
                if (aspectRatio > 0.8 && aspectRatio < 1.2)
                {
                    vector<Point> approx;
                    approxPolyDP(contour, approx, arcLength(contour, true) * 0.04, true);
                    if (approx.size() > 5)
                    { // 輪郭が円に近いかどうかを確認
                        // 円の最小外接円の中心と半径を計算
                        Point2f center;
                        float radius;
                        minEnclosingCircle(contour, center, radius);

                        // 半径フィルタリング (小さすぎるまたは大きすぎる円を除外)
                        if (radius > 10 && radius < 50)
                        {
                            // 円を描画
                            circle(draw, center, (int)radius, Scalar(0, 255, 0), 2);
                            putText(draw, "Ball detected!!", Point(10, 30),
                                    FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 255));
                        }
                    }
                }
            }
        }

        // フレームを4分割
        int rows = draw.rows;
        int cols = draw.cols;
        Mat top_left(draw, Rect(0, 0, cols / 2, rows / 2));
        Mat top_right(draw, Rect(cols / 2, 0, cols / 2, rows / 2));
        Mat bottom_left(draw, Rect(0, rows / 2, cols / 2, rows / 2));
        Mat bottom_right(draw, Rect(cols / 2, rows / 2, cols / 2, rows / 2));

        // 4分割された画像を結合
        Mat top, bottom, combined;
        hconcat(top_left, top_right, top);
        hconcat(bottom_left, bottom_right, bottom);
        vconcat(top, bottom, combined);

        // 4分割された画像の横に元のフレームを結合
        Mat final_output;
        hconcat(draw, combined, final_output);

        imshow("Ball Detection and Quadrants", final_output);

        a = b.clone();
        b = c.clone();

        if (waitKey(30) == 27) // 'ESC'キーが押されたら終了
            break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}