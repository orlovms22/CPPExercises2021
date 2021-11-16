#include "hough.h"

#include <libutils/rasserts.h>
#include <float.h>

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

std::vector<PolarLineExtremum> filterStrongLines(std::vector<PolarLineExtremum> allLines, double thresholdFromWinner)
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
            strongLines.push_back(line);
        }
    }

    return strongLines;
}
