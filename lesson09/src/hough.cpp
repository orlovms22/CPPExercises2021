#include "hough.h"

#include <libutils/rasserts.h>

#include <opencv2/imgproc.hpp>

#define PI 3.14159265358979323846264338327950288
#define DEG_TO_RAD(x) (x) * PI / 180

double estimateR(double x0, double y0, double theta0radians)
{
    double r0 = x0 * cos(theta0radians) + y0 * sin(theta0radians);
    return r0;
}


cv::Mat buildHough(cv::Mat sobel) {// единственный аргумент - это результат свертки Собелем изначальной картинки
    // проверяем что входная картинка - одноканальная и вещественная:
    rassert(sobel.type() == CV_32FC1, 237128273918006);

    int width = sobel.cols;
    int height = sobel.rows;

    // решаем какое максимальное значение у параметра theta в нашем пространстве прямых
    int max_theta = 360;

    // решаем какое максимальное значение у параметра r в нашем пространстве прямых:
    int max_r = sqrt(width * width + height * height) + 1; // TODO замените это число так как вам кажется правильным (отталкиваясь от разрешения картинки - ее ширины и высоты)

    // создаем картинку-аккумулятор, в которой мы будем накапливать суммарные голоса за прямые
    // так же известна как пространство Хафа
    cv::Mat accumulator(max_r, max_theta, CV_32FC1, 0.0f); // TODO подумайте какого разрешения она должна быть и поправьте ее размер
    // TODO не забудьте заполнить эту матрицу-картинку-аккумулятор нулями (очистить)

    // проходим по всем пикселям нашей картинки (уже свернутой оператором Собеля)
    for (int x0 = 0; x0 < height; x0++) {
        for (int y0 = 0; y0 < width; y0++) {
            // смотрим на пиксель с координатами (x0, y0)
            float strength = sobel.at<float>(x0, y0) / FLT_MAX;// TODO считайте его "силу градиента" из картинки sobel

            // теперь для текущего пикселя надо найти все возможные прямые которые через него проходят
            // переберем параметр theta по всему возможному диапазону (в градусах):
            for (int theta0 = 0; theta0 < max_theta - 1; theta0++) {
                // TODO рассчитайте на базе информации о том какие координаты у пикселя - (x0, y0) и какой параметр theta0 мы сейчас рассматриваем
                // TODO обратите внимание что функции sin/cos принимают углы в радианах, поэтому сначала нужно пересчитать theta0 в радианы (воспользуйтесь константой PI)
                float r0 = x0 * cos(DEG_TO_RAD(theta0)) + y0 * sin(DEG_TO_RAD(theta0));
                int theta1 = theta0 + 1;
                float r1 = x0 * cos(DEG_TO_RAD(theta1)) + y0 * sin(DEG_TO_RAD(theta1));

                float fromR = std::min(r0, r1);
                float toR = std::max(r0, r1);

                // TODO теперь рассчитайте координаты пикслея в пространстве Хафа (в картинке-аккумуляторе) соответсвующего параметрам theta0, r0

                if (fromR > 0 && toR > 0) {
                    // чтобы проверить не вышли ли мы за пределы картинки-аккумулятора - давайте явно это проверим:
                    rassert(fromR < accumulator.rows, 237891731289044);
                    rassert(toR < accumulator.rows, 237891731289045);
                    rassert(theta0 >= 0, 237891731289046);
                    rassert(theta0 + 1 < accumulator.cols, 237891731289047);
                    // теперь легко отладить случай выхода за пределы картинки
                    // TODO просто поставьте точку остановки внутри rassert:
                    // нажмите Ctrl+Shift+N -> rasserts.cpp
                    // и поставьте точку остановки на 8 строке: "return line;"

                    // TODO и добавьте в картинку-аккумулятор наш голос с весом strength (взятый из картинки свернутой Собелем)
                    //accumulator.at<float>(r0, theta0) += strength;
                    for (int r = fromR; r < toR; r++)
                    {
                        //accumulator.at<float>(r, theta0) += strength / (toR - fromR);
                        //accumulator.at<float>(r, theta1) += strength / (toR - fromR);
                        accumulator.at<float>(r, theta0) += strength;
                        accumulator.at<float>(r, theta1) += strength;
                        /*float k = (r - fromR) / (toR - fromR);
                        if (r0 >= r1)
                        {
                            accumulator.at<float>(r, theta0) += strength * k;
                            accumulator.at<float>(r, theta1) += strength * (1 - k);
                        }
                        else
                        {
                            accumulator.at<float>(r, theta0) += strength * (1 - k);
                            accumulator.at<float>(r, theta1) += strength * k;
                        }*/
                    }
                }
            }
        }
    }

    return accumulator;
}

std::vector<PolarLineExtremum> findLocalExtremums(cv::Mat houghSpace)
{
    rassert(houghSpace.type() == CV_32FC1, 234827498237080);

    const int max_theta = 360;
    rassert(houghSpace.cols == max_theta, 233892742893082);
    const int max_r = houghSpace.rows;

    const int rad = 1;

    std::vector<PolarLineExtremum> winners;

    for (int theta = rad; theta < max_theta - rad; theta++) {
        for (int r = rad; r < max_r - rad; r++) {
            bool flag = true;
            for (int i = -rad; i < rad; i++)
            {
                for (int j = -rad; j < rad; j++)
                {
                    if (houghSpace.at<float>(r + i, theta + j) > houghSpace.at<float>(r, theta))
                    {
                        flag = false;
                    }
                }
            }
            if (flag) {
                PolarLineExtremum line(theta, r, houghSpace.at<float>(r, theta));
                winners.push_back(line);
            }
        }
    }

    return winners;
}

std::vector<PolarLineExtremum> filterStrongLines(std::vector<PolarLineExtremum> allLines, double thresholdFromWinner, int dist)
{
    std::vector<PolarLineExtremum> strongLines;

    double max_votes = 0;
    for (int i = 0; i < allLines.size(); i++)
    {
        max_votes = std::max(max_votes, allLines[i].votes);
    }
    for (int i = 0; i < allLines.size(); i++)
    {
        if (allLines[i].votes > max_votes * thresholdFromWinner)
        {
            PolarLineExtremum line(allLines[i].theta, allLines[i].r, allLines[i].votes);
            bool flag = true;
            int index = 0;
            for (int j = strongLines.size() - 1; j >= 0; j--)
            {
                if ((abs(strongLines[j].theta - line.theta) < 5 || abs(strongLines[j].theta - line.theta + 360) < 5) && abs(strongLines[j].r - line.r) < dist)
                {
                    flag = false;
                    index = j;
                    break;
                }
            }
            if (flag) strongLines.push_back(line);
            else
            {
                strongLines[index].theta = (strongLines[index].theta * strongLines[index].votes + line.theta * line.votes) / (strongLines[index].votes + line.votes);
                strongLines[index].r = (strongLines[index].r * strongLines[index].votes + line.r * line.votes) / (strongLines[index].votes + line.votes);
            }
            //strongLines.push_back(line);
        }
    }

    return strongLines;
}

cv::Mat erode(cv::Mat mask, uint8_t r, float max_accamulated)
{
    cv::Mat mask_cp = mask.clone();
    for (int i = 0; i < mask.rows; i++)
    {
        for (int j = 0; j < mask.cols; j++)
        {
            if (mask.at<float>(i, j) > max_accamulated / 2)
            {
                for (int dx = -r; dx < r; dx++)
                {
                    for (int dy = -r; dy < r; dy++)
                    {
                        if (i + dx > 0 && i + dx < mask.rows && j + dy > 0 && j + dy < mask.cols)
                        {
                            mask_cp.at<float>(i, j) *= mask.at<float>(i + dx, j + dy) / max_accamulated;
                        }
                    }
                }
            }
        }
    }

    return mask_cp;
}

cv::Mat drawCirclesOnExtremumsInHoughSpace(cv::Mat houghSpace, std::vector<PolarLineExtremum> lines, int radius)
{
    // TODO Доделайте эту функцию - пусть она скопирует картинку с пространством Хафа и отметит на ней красным кружком указанного радиуса (radius) места где были обнаружены экстремумы (на базе списка прямых)

    // делаем копию картинки с пространством Хафа (чтобы не портить само пространство Хафа)
    cv::Mat houghSpaceWithCrosses = houghSpace.clone();

    // проверяем что пространство состоит из 32-битных вещественных чисел (т.е. картинка одноканальная)
    rassert(houghSpaceWithCrosses.type() == CV_32FC1, 347823472890137);

    // но мы хотим рисовать КРАСНЫЙ кружочек вокруг найденных экстремумов, а значит нам не подходит черно-белая картинка
    // поэтому ее надо преобразовать в обычную цветную BGR картинку
    cv::cvtColor(houghSpaceWithCrosses, houghSpaceWithCrosses, cv::COLOR_GRAY2BGR);
    // проверяем что теперь все хорошо и картинка трехканальная (но при этом каждый цвет - 32-битное вещественное число)
    rassert(houghSpaceWithCrosses.type() == CV_32FC3, 347823472890148);

    for (int i = 0; i < lines.size(); i++) {
        PolarLineExtremum line = lines[i];

        // Пример как рисовать кружок в какой-то точке (закомментируйте его):
        //cv::Point point(100, 50);
        //cv::Scalar color(0, 0, 255); // BGR, т.е. красный цвет
        //cv::circle(houghSpaceWithCrosses, point, 3, color);
        cv::circle(houghSpaceWithCrosses, cv::Point(line.theta, line.r), radius, cv::Scalar(0, 0, 255));
        // TODO отметьте в пространстве Хафа красным кружком радиуса radius экстремум соответствующий прямой line
    }

    return houghSpaceWithCrosses;
}

cv::Point PolarLineExtremum::intersect(PolarLineExtremum that)
{
    // Одна прямая - наш текущий объект (this) у которого был вызван этот метод, у этой прямой такие параметры:
    double theta0 = this->theta;
    double r0 = this->r;

    // Другая прямая - другой объект (that) который был передан в этот метод как аргумент, у этой прямой такие параметры:
    double theta1 = that.theta;
    double r1 = that.r;

    double A0 = cos(DEG_TO_RAD(theta0));
    double B0 = sin(DEG_TO_RAD(theta0));

    double A1 = cos(DEG_TO_RAD(theta1));
    double B1 = sin(DEG_TO_RAD(theta1));
    // TODO реализуйте поиск пересечения этих двух прямых, напоминаю что формула прямой описана тут - https://www.polarnick.com/blogs/239/2021/school239_11_2021_2022/2021/11/02/lesson8-hough-transform.html
    // после этого загуглите как искать пересечение двух прямых, пример запроса: "intersect two 2d lines"
    // и не забудьте что cos/sin принимают радианы (используйте toRadians)

    int x = (B0 * (-r1) - B1 * (-r0)) / (A0 * B1 - A1 * B0);
    int y = ((-r0) * A1 - (-r1) * A0) / (A0 * B1 - A1 * B0);

    return cv::Point(y, x);
}

// TODO Реализуйте эту функцию - пусть она скопирует картинку и отметит на ней прямые в соответствии со списком прямых
cv::Mat drawLinesOnImage(cv::Mat img, std::vector<PolarLineExtremum> lines)
{
    // делаем копию картинки (чтобы при рисовании не менять саму оригинальную картинку)
    cv::Mat imgWithLines = img.clone();

    // проверяем что картинка черно-белая (мы ведь ее такой сделали ради оператора Собеля) и 8-битная
    rassert(imgWithLines.type() == CV_8UC1, 45728934700167);

    // но мы хотим рисовать КРАСНЫЕ прямые, а значит нам не подходит черно-белая картинка
    // поэтому ее надо преобразовать в обычную цветную BGR картинку с 8 битами в каждом пикселе
    cv::cvtColor(imgWithLines, imgWithLines, cv::COLOR_GRAY2BGR);
    rassert(imgWithLines.type() == CV_8UC3, 45728934700172);

    // выпишем размер картинки
    int width = imgWithLines.cols;
    int height = imgWithLines.rows;

    for (int i = 0; i < lines.size(); i++) {
        PolarLineExtremum line = lines[i];

        // нам надо найти точки на краях картинки
        cv::Point pointA;
        cv::Point pointB;

        // TODO создайте четыре прямых соответствующих краям картинки (на бумажке нарисуйте картинку и подумайте какие theta/r должны быть у прямых?):
        // напоминаю - чтобы посмотреть какие аргументы требует функция (или в данном случае конструктор объекта) - нужно:
        // 1) раскомментировать эти четыре строки ниже
        // 2) поставить каретку (указатель где вы вводите новые символы) внутри скобок функции (или конструктора, т.е. там где были три вопроса: ???)
        // 3) нажать Ctrl+P чтобы показать список параметров (P=Parameters)
        PolarLineExtremum leftImageBorder(90, 0, 0);
        PolarLineExtremum bottomImageBorder(0, 0, 0);
        PolarLineExtremum rightImageBorder(90, width, 0);
        PolarLineExtremum topImageBorder(0, height, 0);

        // TODO воспользуйтесь недавно созданной функций поиска пересечения прямых чтобы найти точки пересечения краев картинки:
        if (line.theta < 45 || (line.theta > 225 && line.theta < 315))
        {
            pointA = line.intersect(leftImageBorder);
            pointB = line.intersect(rightImageBorder);
        }
        else
        {
            // TODO а в каких случаях нужно использовать пересечение с верхним и нижним краем картинки?
            pointA = line.intersect(bottomImageBorder);
            pointB = line.intersect(topImageBorder);
        }

        // TODO переделайте так чтобы цвет для каждой прямой был случайным (чтобы легче было различать близко расположенные прямые)
        cv::Scalar color(0, 0, 255);
        cv::line(imgWithLines, pointA, pointB, color);
    }

    return imgWithLines;
}
