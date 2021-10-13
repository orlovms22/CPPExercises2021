#include "morphology.h"

cv::Mat makeMask(cv::Mat image, cv::Mat PreImage)
{
    cv::Mat mask(image.rows, image.cols, CV_8UC1, cv::Scalar(0));
    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            uint8_t B = PreImage.at<cv::Vec3b>(i, j)[0];
            uint8_t G = PreImage.at<cv::Vec3b>(i, j)[1];
            uint8_t R = PreImage.at<cv::Vec3b>(i, j)[2];
            uint8_t b = image.at<cv::Vec3b>(i, j)[0];
            uint8_t g = image.at<cv::Vec3b>(i, j)[1];
            uint8_t r = image.at<cv::Vec3b>(i, j)[2];
            if (abs(B - b) < 20 && abs(G - g) < 20 && abs(R - r) < 20)
            {
                mask.at<uint8_t>(i, j) = 255;
            }
            else
            {
                mask.at<uint8_t>(i, j) = 0;
            }
        }
    }

    cv::imshow("mask", mask);
    return mask;
}

cv::Mat maskMorphology(cv::Mat mask, uint8_t dilate_r, uint8_t erode_r)
{
    mask = dilate(mask.clone(), dilate_r);
    mask = erode(mask.clone(), erode_r);
    cv::imshow("mask_morphed", mask);
    return mask;
}

cv::Mat maskDisjointSet(cv::Mat mask, uint16_t min_area)
{
    DisjointSet *areas = new DisjointSet(mask.rows * mask.cols);
    for (int i = 1; i < mask.rows; i++)
    {
        for (int j = 1; j < mask.cols; j++)
        {
            if (mask.at<uint8_t>(i, j) == 255)
            {
                if (mask.at<uint8_t>(i, j - 1) == 255)
                {
                    if (areas->get_set(mask.cols * i + j) != areas->get_set(mask.cols * i + j - 1))
                        areas->union_sets(mask.cols * i + j, mask.cols * i + j - 1);
                }
                if (mask.at<uint8_t>(i - 1, j) == 255)
                {
                    if (areas->get_set(mask.cols * i + j) != areas->get_set(mask.cols * (i - 1) + j))
                        areas->union_sets(mask.cols * i + j, mask.cols * (i - 1) + j);
                }
            }
        }
    }

    for (int i = 0; i < mask.rows; i++)
    {
        for (int j = 0; j < mask.cols; j++)
        {
            if (areas->get_set_size(mask.cols * i + j) < min_area)
            {
                mask.at<uint8_t>(i, j) = 0;
            }
        }
    }
    delete areas;
    cv::imshow("disjoint_set_mask", mask);
    return mask;
}

cv::Mat fillBackground(cv::Mat image, cv::Mat background, cv::Mat mask)
{
    for (int i = 0; i < mask.rows; i++)
    {
        for (int j = 0; j < mask.cols; j++)
        {
            if (mask.at<uint8_t>(i, j) == 255)
            {
                uint16_t x_new = i * background.rows / image.rows;
                uint16_t y_new = j * background.cols / image.cols;
                image.at<cv::Vec3b>(i, j)[0] = background.at<cv::Vec3b>(x_new, y_new)[0];
                image.at<cv::Vec3b>(i, j)[1] = background.at<cv::Vec3b>(x_new, y_new)[1];
                image.at<cv::Vec3b>(i, j)[2] = background.at<cv::Vec3b>(x_new, y_new)[2];
            }
        }
    }

    return image;
}

cv::Mat dilate(cv::Mat mask, uint8_t r)
{
    cv::Mat mask_cp = mask.clone();
    for (int i = 0; i < mask.rows; i++)
    {
        for (int j = 0; j < mask.cols; j++)
        {
            if (mask.at<uint8_t>(i, j) == 0)
            {
                for (int dx = -r; dx < r; dx++)
                {
                    for (int dy = -r; dy < r; dy++)
                    {
                        if (i + dx > 0 && i + dx < mask.rows && j + dy > 0 && j + dy < mask.cols)
                        {
                            if (mask.at<uint8_t>(i + dx, j + dy) == 255)
                            {
                                mask_cp.at<uint8_t>(i, j) = 255;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    return mask_cp;
}

cv::Mat erode(cv::Mat mask, uint8_t r)
{
    cv::Mat mask_cp = mask.clone();
    for (int i = 0; i < mask.rows; i++)
    {
        for (int j = 0; j < mask.cols; j++)
        {
            if (mask.at<uint8_t>(i, j) == 1)
            {
                for (int dx = -r; dx < r; dx++)
                {
                    for (int dy = -r; dy < r; dy++)
                    {
                        if (i + dx > 0 && i + dx < mask.rows && j + dy > 0 && j + dy < mask.cols)
                        {
                            mask_cp.at<uint8_t>(i, j) *= mask.at<uint8_t>(i + dx, j + dy);
                        }
                    }
                }
            }
        }
    }

    return mask_cp;
}
