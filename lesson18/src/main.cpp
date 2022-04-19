#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <set>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <memory>

#include <libutils/rasserts.h>
#include <libutils/fast_random.h>

#define sq(x) (x)*(x)

// Эта функция говорит нам правда ли пиксель отмаскирован, т.е. отмечен как "удаленный", т.е. белый
bool isPixelMasked(cv::Mat mask, int j, int i) {
    rassert(j >= 0 && j < mask.rows, 372489347280017);
    rassert(i >= 0 && i < mask.cols, 372489347280018);
    rassert(mask.type() == CV_8UC3, 2348732984792380019);

    // TODO проверьте белый ли пиксель
    cv::Vec3b pixel = mask.at<cv::Vec3b>(j, i);
    return pixel[0] > 0 && pixel[1] > 0 && pixel[2] > 0;
}

double pixelQuality(cv::Vec3b a, cv::Vec3b b) //возвращает квадрат разности пикселей
{
    return sq(a[0] - b[0]) + sq(a[1] - b[1]) + sq(a[2] - b[2]);
}

double estimateQuality(cv::Mat image, int x, int y, int nx, int ny, uint8_t size_x, uint8_t size_y)
{
    rassert(x >= 0 && x < image.rows, 372489347280017);
    rassert(y >= 0 && y < image.cols, 372489347280018);
    rassert(nx >= 0 && nx < image.rows, 372489347280019);
    if (ny == image.cols) return 0;
    rassert(ny >= 0 && ny < image.cols, 372489347280020);

    double diff = 0;
    for (int i = 0; i < size_x; i++) {
        for (int j = 0; j < size_y; j++) {
            int pixel_x = i - size_x / 2;
            int pixel_y = j - size_y / 2;
            diff += pixelQuality(image.at<cv::Vec3b>(x + pixel_x, y + pixel_y), image.at<cv::Vec3b>(nx + pixel_x, ny + pixel_y));
        }
    }
    diff = sqrt(diff);
    diff /= (sq(255) * 3 * size_x * size_y);
    diff = 1.0 - diff;
    return diff;
}

void run(int caseNumber, std::string caseName) {
    std::cout << "_________Case #" << caseNumber << ": " <<  caseName << "_________" << std::endl;

    cv::Mat original = cv::imread("../../../../lesson18/data/" + std::to_string(caseNumber) + "_" + caseName + "/" + std::to_string(caseNumber) + "_original.jpg");
    cv::Mat mask = cv::imread("../../../../lesson18/data/" + std::to_string(caseNumber) + "_" + caseName + "/" + std::to_string(caseNumber) + "_mask.png");
    rassert(!original.empty(), 324789374290018);
    rassert(!mask.empty(), 378957298420019);

    // TODO напишите rassert сверяющий разрешение картинки и маски
    // TODO выведите в консоль это разрешение картинки
    rassert(mask.rows == original.rows, 49375039473329047);
    rassert(mask.cols == original.cols, 49375039473329048);
    std::cout << "Image resolution: " << original.rows << "x" << original.cols << std::endl;

    // создаем папку в которую будем сохранять результаты - lesson18/resultsData/ИМЯ_НАБОРА/
    std::string resultsDir = "../../../../lesson18/resultsData/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }
    resultsDir += std::to_string(caseNumber) + "_" + caseName + "/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }

    // сохраняем в папку с результатами оригинальную картинку и маску
    cv::imwrite(resultsDir + "0original.png", original);
    cv::imwrite(resultsDir + "1mask.png", mask);

    // TODO замените белым цветом все пиксели в оригинальной картинке которые покрыты маской
    // TODO сохраните в папку с результатами то что получилось под названием "2_original_cleaned.png"
    // TODO посчитайте и выведите число отмаскированных пикселей (числом и в процентах) - в таком формате:
    // Number of masked pixels: 7899/544850 = 1%

    int masked_pixels = 0;
    int all_pixels = mask.rows * mask.cols;
    for (int i = 0; i < mask.rows; i++) {
        for (int j = 0; j < mask.cols; j++) {
            if (isPixelMasked(mask, i, j)) {
                original.at<cv::Vec3b>(i, j)[0] = 255;
                original.at<cv::Vec3b>(i, j)[1] = 255;
                original.at<cv::Vec3b>(i, j)[2] = 255;
                masked_pixels++;
            }
        }
    }
    cv::imwrite(resultsDir + "2original_cleaned.png", original);
    std::cout << "Number of masked pixels: " << masked_pixels << "/" << all_pixels << " = " << 100 * masked_pixels / all_pixels << "%" << std::endl;

    FastRandom random(32542341); // этот объект поможет вам генерировать случайные гипотезы

    // TODO 10 создайте картинку хранящую относительные смещения - откуда брать донора для заплатки, см. подсказки про то как с нею работать на сайте
    // TODO 11 во всех отмаскированных пикселях: заполните эту картинку с относительными смещениями - случайными смещениями (но чтобы они и их окрестность 5х5 не выходила за пределы картинки)
    // TODO 12 во всех отмаскированных пикселях: замените цвет пиксела А на цвет пикселя Б на который указывает относительное смещение пикселя А
    // TODO 13 сохраните получившуюся картинку на диск
    // TODO 14 выполняйте эти шаги 11-13 много раз, например 1000 раз (оберните просто в цикл, сохраняйте картинку на диск только на каждой десятой или сотой итерации)
    // TODO 15 теперь давайте заменять значение относительного смещения на новой только если новая случайная гипотеза - лучше старой, добавьте оценку "насколько смещенный патч 5х5 похож на патч вокруг пикселя если их наложить"
    //
    // Ориентировочный псевдокод-подсказка получившегося алгоритма:
    cv::Mat shifts(mask.rows, mask.cols, CV_32SC2); // матрица хранящая смещения, изначально заполнена парами нулей
    cv::Mat image = original; // текущая картинка
    for (int cnt = 0; cnt < 1000; cnt++) {
        for (int i = 0; i < mask.rows; i++) {
            for (int j = 0; j < mask.cols; j++) {
                if (!isPixelMasked(mask, i, j))
                    continue; // пропускаем т.к. его менять не надо
                cv::Vec2i dxy = shifts.at< cv::Vec2i>(i, j); // смотрим какое сейчас смещение для этого пикселя в матрице смещения
                int nx = i + dxy[0];
                int ny = j + dxy[1];
                double currentQuality = estimateQuality(image, i, j, nx, ny, 5, 5); // эта функция (создайте ее) считает насколько похож квадрат 5х5 приложенный центром к (i, j)
                // на квадрат 5х5 приложенный центром к(nx, ny)

                int rx = random.next(-i + 3, mask.rows - i - 3); // создаем случайное смещение относительно нашего пикселя, воспользуйтесь функцией random.next(...);
                int ry = random.next(-j + 3, mask.cols - j - 3);// (окрестность вокруг пикселя на который укажет смещение - не должна выходить за пределы картинки и не должна быть отмаскирована)
                double randomQuality = estimateQuality(image, i, j, i + rx, j + ry, 5, 5); // оцениваем насколько похоже будет если мы приложим эту случайную гипотезу которую только что выбрали

                if (randomQuality > currentQuality || (dxy[0] == 0 && dxy[1] == 0)) { //если новое качество случайной угадайки оказалось лучше старого
                    shifts.at< cv::Vec2i>(i, j)[0] = rx;//то сохраняем(rx, ry) в картинку смещений
                    shifts.at< cv::Vec2i>(i, j)[1] = ry;
                    image.at<cv::Vec3b>(i, j) = image.at<cv::Vec3b>(i + rx, j + ry);// и в текущем пикселе кладем цвет из пикселя на которого только что смотрели(цент окрестности по смещению)
                    // (т.е.мы не весь патч сюда кладем, а только его центральный пиксель)
                }
                else {
                    // а что делать если новая случайная гипотеза хуже чем то что у нас уже есть ?
                    if (i > 0) {
                        if (isPixelMasked(mask, i - 1, j)) {
                            rx = shifts.at< cv::Vec2i>(i - 1, j)[0];
                            ry = shifts.at< cv::Vec2i>(i - 1, j)[1];
                            randomQuality = estimateQuality(image, i, j, i + rx, j + ry, 5, 5);
                            if (randomQuality > currentQuality) {
                                shifts.at< cv::Vec2i>(i, j)[0] = rx;
                                shifts.at< cv::Vec2i>(i, j)[1] = ry;
                                image.at<cv::Vec3b>(i, j) = image.at<cv::Vec3b>(i + rx, j + ry);
                            }
                        }
                    }
                    if (j > 0) {
                        if (isPixelMasked(mask, i, j - 1)) {
                            rx = shifts.at< cv::Vec2i>(i, j - 1)[0];
                            ry = shifts.at< cv::Vec2i>(i, j - 1)[1];
                            randomQuality = estimateQuality(image, i, j, i + rx, j + ry, 5, 5);
                            if (randomQuality > currentQuality) {
                                shifts.at< cv::Vec2i>(i, j)[0] = rx;
                                shifts.at< cv::Vec2i>(i, j)[1] = ry;
                                image.at<cv::Vec3b>(i, j) = image.at<cv::Vec3b>(i + rx, j + ry);
                            }
                        }
                    }
                }
            }
        }
        //не забываем сохранить на диск текущую картинку
        //а как численно оценить насколько уже хорошую картинку мы смогли построить? выведите в консоль это число
    }
    cv::imwrite(resultsDir + "3retouched.png", image);
}


int main() {
    try {
        //run(1, "mic");
        // TODO протестируйте остальные случаи:
//        run(2, "flowers");
//        run(3, "baloons");
//        run(4, "brickwall");
        run(5, "old_photo");
//        run(6, "your_data"); // TODO придумайте свой случай для тестирования (рекомендуется не очень большое разрешение, например 300х300)

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
