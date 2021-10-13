#pragma once

#include <opencv2/highgui.hpp>
#include "disjoint_set.h"

cv::Mat makeMask(cv::Mat image, cv::Mat PreImage);

cv::Mat maskMorphology(cv::Mat mask, uint8_t dilate_r, uint8_t erode_r);

cv::Mat maskDisjointSet(cv::Mat mask, uint16_t min_area);

cv::Mat fillBackground(cv::Mat image, cv::Mat background, cv::Mat mask);

cv::Mat dilate(cv::Mat mask, uint8_t r);

cv::Mat erode(cv::Mat mask, uint8_t r);
