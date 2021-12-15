#include "parseSymbols.h"
#include <libutils/rasserts.h>

struct image_position
{
    cv::Mat image;
    cv::Point position;

    image_position(cv::Mat img, cv::Point pos) : image(img), position(pos)
    {

    }
};

std::vector<cv::Mat> splitSymbols(cv::Mat img)
{
    std::vector<cv::Mat> symbols;
    // TODO 101: чтобы извлечь кусок картинки (для каждого прямоугольника cv::Rect вокруг символа) - загуглите "opencv how to extract subimage"
    cv::cvtColor(img.clone(), img, cv::COLOR_BGR2GRAY);

    cv::Mat binary;
    cv::adaptiveThreshold(img, binary, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 81, 20);

    cv::Mat binary_dilated;
    cv::dilate(binary, binary_dilated, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
    binary = binary_dilated;

    std::vector<std::vector<cv::Point>> contoursPoints2;
    cv::findContours(binary, contoursPoints2, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    cv::Mat result_image;
    cv::threshold(binary, result_image, 128, 255, cv::THRESH_BINARY_INV);
    std::vector<image_position> symbols_positions;
    for (int contourI = 0; contourI < contoursPoints2.size(); ++contourI) {
        std::vector<cv::Point> points = contoursPoints2[contourI]; // перем очередной контур
        cv::Rect box = cv::boundingRect(points); // строим прямоугольник по всем пикселям контура (bounding box = бокс ограничивающий объект)
        cv::Point top_left = box.tl();
        cv::Size size = box.size();
        if(box.x >= 5) box.x = top_left.x - 5;
        if (box.y >= 5) box.y = top_left.y - 5;
        cv::Size image_size = result_image.size();
        if((box.x + box.width) < (image_size.width - 10)) box.width = size.width + 10;
        if ((box.y + box.height) < (image_size.height - 10))box.height = size.height + 10;
        cv::Mat symbol = result_image(box);
        top_left = box.tl();
        symbols_positions.push_back(image_position(symbol, top_left));
        //symbols.push_back(symbol);
        //cv::imshow("abc", symbol);
        //cv::waitKey(0);
    }
    sort(symbols_positions.begin(), symbols_positions.end(), [](const image_position& left, const image_position& right)
    {
         return left.position.x < right.position.x;
    });

    for (int i = 0; i < symbols_positions.size(); i++)
    {
        symbols.push_back(symbols_positions[i].image);
    }

    return symbols;
}

#define _USE_MATH_DEFINES
#include <math.h>

#define pow2(x) (x) * (x)

HoG buildHoG(cv::Mat grad_x, cv::Mat grad_y) {
    rassert(grad_x.type() == CV_32FC1, 2378274892374008);
    rassert(grad_y.type() == CV_32FC1, 2378274892374008);

    rassert(grad_x.rows == grad_y.rows, 3748247980010);
    rassert(grad_x.cols == grad_y.cols, 3748247980011);
    int height = grad_x.rows;
    int width = grad_x.cols;

    HoG hog;
    hog.resize(NBINS);
    fill(hog.begin(), hog.end(), 0);

    double sum = 0;
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            float dx = grad_x.at<float>(j, i);
            float dy = grad_y.at<float>(j, i);
            float strength = sqrt(dx * dx + dy * dy);
            double angle = atan2(dy, dx) * 180 / M_PI;
            if (angle < 0) angle += 360;

            if (strength < 10) // пропускайте слабые градиенты, это нужно чтобы игнорировать артефакты сжатия в jpeg (например в line01.jpg пиксели не идеально белые/черные, есть небольшие отклонения)
                continue;

            // TODO рассчитайте в какую корзину нужно внести голос
            int bin = int(angle) / int(360 / NBINS);

            rassert(bin >= 0, 3842934728039);
            rassert(bin < NBINS, 34729357289040);
            hog[bin] += strength;
            sum += strength;
        }
    }

    for (int bin = 0; bin < NBINS; bin++)
    {
        hog[bin] /= sum;
    }

    rassert(hog.size() == NBINS, 23478937290010);
    return hog;
}


HoG buildHoG(cv::Mat originalImg) {
    cv::Mat img = originalImg.clone();

    rassert(img.type() == CV_8UC3, 347283678950077);

    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY); // преобразуем в оттенки серого

    cv::Mat grad_x, grad_y; // в этих двух картинках мы получим производную (градиент=gradient) по оси x и y
    // для этого достаточно дважды применить оператор Собеля (реализованный в OpenCV)
    cv::Sobel(img, grad_x, CV_32FC1, 1, 0);
    cv::Sobel(img, grad_y, CV_32FC1, 0, 1);
    rassert(!grad_x.empty(), 234892748239070017);
    rassert(!grad_y.empty(), 234892748239070018);

    // TODO реализуйте эту функцию:
    HoG hog = buildHoG(grad_x, grad_y);
    return hog;
}

std::ostream& operator<<(std::ostream& os, const HoG& hog) {
    rassert(hog.size() == NBINS, 234728497230016);

    // TODO
    os << "HoG[";
    for (int bin = 0; bin < NBINS; bin++) {
        os << (double(bin) + 0.5) * 360 / NBINS << "=" << int(hog[bin] * 100) << "%, ";
    }
    os << "]";
    return os;
}

double distance(HoG a, HoG b) {
    rassert(a.size() == NBINS, 237281947230077);
    rassert(b.size() == NBINS, 237281947230078);

    double sum = 0;
    for (int bin = 0; bin < NBINS; bin++)
    {
        sum += pow2(a[bin] - b[bin]);
    }

    double res = sqrt(sum);
    return res;
}
