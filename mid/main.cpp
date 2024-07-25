#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;

int main()
{
    int thresh = 25;     // 閾値を設定します。
    int max_diff = 10;   // 最大の差分を設定します。
    int frame_count = 0; // フレームカウントを初期化します。

    Mat a, b, c, draw;                   // Matオブジェクトを作成します。a, b, cは連続する3つのフレームを保存し、drawは結果を描画するための画像を保存します。
    VideoCapture cap(0);                 // ウェブカメラを開いてビデオをキャプチャします。
    cap.set(CAP_PROP_FRAME_WIDTH, 480);  // キャプチャするビデオのフレーム幅を設定します。
    cap.set(CAP_PROP_FRAME_HEIGHT, 320); // キャプチャするビデオのフレーム高さを設定します。

    if (cap.isOpened()) // ウェブカメラが正常に開かれたかどうかを確認します。
    {
        cap >> a; // 最初のフレームをキャプチャします。
        cap >> b; // 2番目のフレームをキャプチャします。

        while (true) // 無限ループを開始します。
        {
            cap >> c;      // 3番目のフレームをキャプチャします。
            if (c.empty()) // フレームが空かどうかを確認します。空の場合、ループを終了します。
            {
                break;
            }
            draw = c.clone(); // 現在のフレームをクローンしてdrawに保存します。

            Mat a_gray, b_gray, c_gray, diff1, diff2, diff1_t, diff2_t, diff; // 必要なMatオブジェクトを作成します。
            cvtColor(a, a_gray, COLOR_BGR2GRAY);                              // 最初のフレームをグレースケールに変換します。
            cvtColor(b, b_gray, COLOR_BGR2GRAY);                              // 2番目のフレームをグレースケールに変換します。
            cvtColor(c, c_gray, COLOR_BGR2GRAY);                              // 3番目のフレームをグレースケールに変換します。

            absdiff(a_gray, b_gray, diff1); // 最初と2番目のフレーム間の絶対差分を計算します。
            absdiff(b_gray, c_gray, diff2); // 2番目と3番目のフレーム間の絶対差分を計算します。

            threshold(diff1, diff1_t, thresh, 255, THRESH_BINARY); // diff1に閾値を適用します。
            threshold(diff2, diff2_t, thresh, 255, THRESH_BINARY); // diff2に閾値を適用します。

            bitwise_and(diff1_t, diff2_t, diff); // diff1_tとdiff2_tのビット単位のANDを計算します。

            Mat k = getStructuringElement(MORPH_CROSS, Size(3, 3)); // 形状要素を取得します。
            morphologyEx(diff, diff, MORPH_OPEN, k);                // モルフォロジー変換を適用します。

            int diff_cnt = countNonZero(diff); // diffの非ゼロ要素の数をカウントします。
            if (diff_cnt > max_diff)           // 非ゼロ要素の数がmax_diffより大きい場合、動きが検出されたと判断します。
            {
                std::vector<Point> nzero; // 非ゼロ要素の位置を保存するベクトルを作成します。
                findNonZero(diff, nzero); // diffの非ゼロ要素の位置を検出します。

                Rect bbox = boundingRect(nzero);             // 非ゼロ要素の位置からバウンディングボックスを計算します。
                rectangle(draw, bbox, Scalar(0, 255, 0), 2); // バウンディングボックスを描画します。

                putText(draw, "Motion detected!!", Point(10, 30), // テキストを描画します。
                        FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 255));

                // 動きがある時スクリーンショット
                // std::string filename = "frame_" + std::to_string(frame_count) + ".jpg";
                // cv::imwrite(filename, draw);
                // frame_count++;
            }

            Mat diff_color;                             // diffのカラーバージョンを保存するMatを作成します。
            cvtColor(diff, diff_color, COLOR_GRAY2BGR); // diffをカラーに変換します。
            std::vector<Mat> imgs = {draw, diff_color}; // 描画とdiff_colorをベクトルにまとめます。
            Mat stacked;                                // 画像を水平に連結するためのMatを作成します。
            hconcat(imgs, stacked);                     // 画像を水平に連結します。
            imshow("motion", stacked);                  // 結果を表示します。

            a = b.clone(); // bをクローンしてaに保存します。
            b = c.clone(); // cをクローンしてbに保存します。

            if (waitKey(1) == 27) // ESCキー
            {
                break; // ループを終了します。
            }
        }
    }

    cap.release();       // ビデオキャプチャを解放します。
    destroyAllWindows(); // すべてのウィンドウを破棄します。
    return 0;
}