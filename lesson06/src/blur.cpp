#include "blur.h"

#include <libutils/rasserts.h>

cv::Mat blur(cv::Mat img, double sigma) {
    // TODO формулу весов можно найти тут:
    // https://ru.wikipedia.org/wiki/%D0%A0%D0%B0%D0%B7%D0%BC%D1%8B%D1%82%D0%B8%D0%B5_%D0%BF%D0%BE_%D0%93%D0%B0%D1%83%D1%81%D1%81%D1%83
    int n = sigma * 3;
    std::vector<std::vector<float>> blurKoef;
    blurKoef.resize(n * 2 + 1);
    for (int i = 0; i <= n * 2; i++)
    {
        blurKoef[i].resize(n * 2 + 1);
    }
    for (int x = -n; x <= n; x++)
    {
        for (int y = -n; y <= n; y++)
        {
            blurKoef[x + n][y + n] = exp(-(x * x + y * y) / (2 * sigma * sigma)) / (2 * CV_PI * sigma * sigma);
        }
    }
    cv::Mat resultImg = img.clone();
    int width = img.cols;
    int height = img.rows;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float red = 0;
            float green = 0;
            float blue = 0;
            float sum = 1;
            for (int x = -n; x <= n; x++)
            {
                for (int y = -n; y <= n; y++)
                {
                    int X = i + x;
                    int Y = j + y;
                    if (X < 0 || Y < 0 || X >= height || Y >= width)
                    {
                        sum -= blurKoef[x + n][y + n];
                    }
                    else
                    {
                        red += img.at<cv::Vec3b>(X, Y)[2] * blurKoef[x + n][y + n];
                        green += img.at<cv::Vec3b>(X, Y)[1] * blurKoef[x + n][y + n];
                        blue += img.at<cv::Vec3b>(X, Y)[0] * blurKoef[x + n][y + n];
                    }
                }
            }
            resultImg.at<cv::Vec3b>(i, j)[2] = uchar(red / sum);
            resultImg.at<cv::Vec3b>(i, j)[1] = uchar(green / sum);
            resultImg.at<cv::Vec3b>(i, j)[0] = uchar(blue / sum);
        }
    }
    return resultImg;
}
