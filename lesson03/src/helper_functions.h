#pragma once

#include <opencv2/highgui.hpp> // подключили часть библиотеки OpenCV, теперь мы можем работать с картинками (знаем про тип cv::Mat)

cv::Mat makeAllBlackPixelsBlue(cv::Mat image);

cv::Mat invertImageColors(cv::Mat image);

cv::Mat addBackgroundInsteadOfBlackPixels(cv::Mat object, cv::Mat background);

cv::Mat addBackgroundInsteadOfBlackPixelsLargeBackground(cv::Mat object, cv::Mat largeBackground);

cv::Mat drawImageOnBackgroundN(cv::Mat object, cv::Mat largeBackground, uint8_t n);

cv::Mat addBackgroundInsteadOfBlackPixelsLargeBackgroundResize(cv::Mat object, cv::Mat largeBackground);

cv::Mat makeAllBlackPixelsRandom(cv::Mat image);

cv::Mat makeBackgroungClear(cv::Mat image, cv::Mat backgroung, int x, int y);

cv::Mat makeBackgroungClearPreMat(cv::Mat image, cv::Mat background, cv::Mat PreImage);

cv::Mat dilate(cv::Mat mask, uint8_t r);

cv::Mat erode(cv::Mat mask, uint8_t r);
