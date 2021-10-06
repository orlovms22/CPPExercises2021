#include "helper_functions.h"

#include <libutils/rasserts.h>
#include <cstdlib>


cv::Mat makeAllBlackPixelsBlue(cv::Mat image) {
    // TODO реализуйте функцию которая каждый черный пиксель картинки сделает синим

    // ниже приведен пример как узнать цвет отдельного пикселя - состоящий из тройки чисел BGR (Blue Green Red)
    // чем больше значение одного из трех чисел - тем насыщеннее его оттенок
    // всего их диапазон значений - от 0 до 255 включительно
    // т.е. один байт, поэтому мы используем ниже тип unsigned char - целое однобайтовое неотрицательное число
    /*cv::Vec3b color = image.at<cv::Vec3b>(13, 5); // взяли и узнали что за цвет в пикселе в 14-ом ряду (т.к. индексация с нуля) и 6-ой колонке
    unsigned char blue = color[0]; // если это число равно 255 - в пикселе много синего, если равно 0 - в пикселе нет синего
    unsigned char green = color[1];
    unsigned char red = color[2];

    // как получить белый цвет? как получить черный цвет? как получить желтый цвет?
    // поэкспериментируйте! например можете всю картинку заполнить каким-то одним цветом

    // пример как заменить цвет по тем же координатам
    red = 255;
    // запустите эту версию функции и посмотрите на получившуюся картинку - lesson03/resultsData/01_blue_unicorn.jpg
    // какой пиксель изменился? почему он не чисто красный?
    image.at<cv::Vec3b>(13, 5) = cv::Vec3b(blue, green, red);*/
    for (int i = 0; i < image.cols; i++)
    {
        for (int j = 0; j < image.rows; j++)
        {
            uint8_t b = image.at<cv::Vec3b>(i, j)[0];
            uint8_t g = image.at<cv::Vec3b>(i, j)[1];
            uint8_t r = image.at<cv::Vec3b>(i, j)[2];
            if (b < 20 && g < 20 && r < 20)
            {
                image.at<cv::Vec3b>(i, j)[0] = 255;
                image.at<cv::Vec3b>(i, j)[1] = 0;
                image.at<cv::Vec3b>(i, j)[2] = 0;
            }
        }
    }

    return image;
}

cv::Mat invertImageColors(cv::Mat image) {
    // TODO реализуйте функцию которая каждый цвет картинки инвертирует:
    // т.е. пусть ночь станет днем, а сумрак рассеется
    // иначе говоря замените каждое значение яркости x на (255-x) (т.к находится в диапазоне от 0 до 255)
    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            uint8_t b = image.at<cv::Vec3b>(i, j)[0];
            uint8_t g = image.at<cv::Vec3b>(i, j)[1];
            uint8_t r = image.at<cv::Vec3b>(i, j)[2];
            image.at<cv::Vec3b>(i, j)[0] = 255 - b;
            image.at<cv::Vec3b>(i, j)[1] = 255 - g;
            image.at<cv::Vec3b>(i, j)[2] = 255 - r;
        }
    }

    return image;
}

cv::Mat addBackgroundInsteadOfBlackPixels(cv::Mat object, cv::Mat background) {
    // TODO реализуйте функцию которая все черные пиксели картинки-объекта заменяет на пиксели с картинки-фона
    // т.е. что-то вроде накладного фона получится

    // гарантируется что размеры картинок совпадают - проверьте это через rassert, вот например сверка ширины:
    rassert(object.cols == background.cols, 341241251251351);
    rassert(object.rows == background.rows, 341241251251351);

    for (int i = 0; i < object.cols; i++)
    {
        for (int j = 0; j < object.rows; j++)
        {
            uint8_t b = object.at<cv::Vec3b>(i, j)[0];
            uint8_t g = object.at<cv::Vec3b>(i, j)[1];
            uint8_t r = object.at<cv::Vec3b>(i, j)[2];
            if (r < 20 && g < 20 && b < 20)
            {
                object.at<cv::Vec3b>(i, j)[0] = background.at<cv::Vec3b>(i, j)[0];
                object.at<cv::Vec3b>(i, j)[1] = background.at<cv::Vec3b>(i, j)[1];
                object.at<cv::Vec3b>(i, j)[2] = background.at<cv::Vec3b>(i, j)[2];
            }
        }
    }

    return object;
}

cv::Mat addBackgroundInsteadOfBlackPixelsLargeBackground(cv::Mat object, cv::Mat largeBackground) {
    // теперь вам гарантируется что largeBackground гораздо больше - добавьте проверок этого инварианта (rassert-ов)

    rassert(object.cols * 4 < largeBackground.cols, 1234567890);
    rassert(object.rows * 4 < largeBackground.rows, 1234567890);
    // TODO реализуйте функцию так, чтобы нарисовался объект ровно по центру на данном фоне, при этом черные пиксели объекта не должны быть нарисованы

    uint16_t dx = (largeBackground.cols - object.cols) / 2;
    uint16_t dy = (largeBackground.rows - object.rows) / 2;

    for (int i = 0; i < object.rows; i++)
    {
        for (int j = 0; j < object.cols; j++)
        {
            uint8_t b = object.at<cv::Vec3b>(i, j)[0];
            uint8_t g = object.at<cv::Vec3b>(i, j)[1];
            uint8_t r = object.at<cv::Vec3b>(i, j)[2];
            if (r > 20 || g > 20 || b > 20)
            {
                largeBackground.at<cv::Vec3b>(i + dy, j + dx)[0] = object.at<cv::Vec3b>(i, j)[0];
                largeBackground.at<cv::Vec3b>(i + dy, j + dx)[1] = object.at<cv::Vec3b>(i, j)[1];
                largeBackground.at<cv::Vec3b>(i + dy, j + dx)[2] = object.at<cv::Vec3b>(i, j)[2];
            }
        }
    }

    return largeBackground;
}

cv::Mat drawImageOnBackgroundN(cv::Mat object, cv::Mat largeBackground, uint8_t n)
{
    for (int k = 0; k < n; k++)
    {
        uint16_t dx = (largeBackground.cols - object.cols - 1) * rand() / RAND_MAX;
        uint16_t dy = (largeBackground.rows - object.rows - 1) * rand() / RAND_MAX;

        for (int i = 0; i < object.rows; i++)
        {
            for (int j = 0; j < object.cols; j++)
            {
                uint8_t b = object.at<cv::Vec3b>(i, j)[0];
                uint8_t g = object.at<cv::Vec3b>(i, j)[1];
                uint8_t r = object.at<cv::Vec3b>(i, j)[2];
                if (r > 20 || g > 20 || b > 20)
                {
                    largeBackground.at<cv::Vec3b>(i + dy, j + dx)[0] = object.at<cv::Vec3b>(i, j)[0];
                    largeBackground.at<cv::Vec3b>(i + dy, j + dx)[1] = object.at<cv::Vec3b>(i, j)[1];
                    largeBackground.at<cv::Vec3b>(i + dy, j + dx)[2] = object.at<cv::Vec3b>(i, j)[2];
                }
            }
        }
    }

    return largeBackground;
}

cv::Mat addBackgroundInsteadOfBlackPixelsLargeBackgroundResize(cv::Mat object, cv::Mat largeBackground)
{
    for (int i = 0; i < largeBackground.rows; i++)
    {
        for (int j = 0; j < largeBackground.cols; j++)
        {
            uint16_t x = i * object.rows / largeBackground.rows;
            uint16_t y = j * object.cols / largeBackground.cols;
            uint8_t b = object.at<cv::Vec3b>(x, y)[0];
            uint8_t g = object.at<cv::Vec3b>(x, y)[1];
            uint8_t r = object.at<cv::Vec3b>(x, y)[2];
            if (r > 20 || g > 20 || b > 20)
            {
                largeBackground.at<cv::Vec3b>(i, j)[0] = object.at<cv::Vec3b>(x, y)[0];
                largeBackground.at<cv::Vec3b>(i, j)[1] = object.at<cv::Vec3b>(x, y)[1];
                largeBackground.at<cv::Vec3b>(i, j)[2] = object.at<cv::Vec3b>(x, y)[2];
            }
        }
    }

    return largeBackground;
}

cv::Mat makeAllBlackPixelsRandom(cv::Mat image)
{
    for (int i = 0; i < image.cols; i++)
    {
        for (int j = 0; j < image.rows; j++)
        {
            uint8_t b = image.at<cv::Vec3b>(i, j)[0];
            uint8_t g = image.at<cv::Vec3b>(i, j)[1];
            uint8_t r = image.at<cv::Vec3b>(i, j)[2];
            if (b < 20 && g < 20 && r < 20)
            {
                image.at<cv::Vec3b>(i, j)[0] = 255 * rand() / RAND_MAX;
                image.at<cv::Vec3b>(i, j)[1] = 255 * rand() / RAND_MAX;
                image.at<cv::Vec3b>(i, j)[2] = 255 * rand() / RAND_MAX;
            }
        }
    }

    return image;
}

cv::Mat makeBackgroungClear(cv::Mat image, cv::Mat background, int x, int y)
{
    rassert(x < image.rows, 1234567890);
    rassert(y < image.cols, 1234567890);

    uint8_t B = image.at<cv::Vec3b>(x, y)[0];
    uint8_t G = image.at<cv::Vec3b>(x, y)[1];
    uint8_t R = image.at<cv::Vec3b>(x, y)[2];

    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            uint8_t b = image.at<cv::Vec3b>(i, j)[0];
            uint8_t g = image.at<cv::Vec3b>(i, j)[1];
            uint8_t r = image.at<cv::Vec3b>(i, j)[2];
            if (abs(B - b) < 20 && abs(G - g) < 20 && abs(R - r) < 20)
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

cv::Mat makeBackgroungClearPreMat(cv::Mat image, cv::Mat background, cv::Mat PreImage)
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
    mask = dilate(mask.clone(), 3);
    mask = erode(mask.clone(), 7);
    cv::imshow("mask_morphed", mask);

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
