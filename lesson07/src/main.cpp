#include <filesystem>
#include <iostream>
#include <libutils/rasserts.h>

#include "hough.h"

#include <opencv2/imgproc.hpp>


void test(std::string name) {
    std::string full_path = "../../../../lesson05/data/" + name + ".jpg";
    cv::Mat img = cv::imread(full_path);
    rassert(!img.empty(), 238982391080010);
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY); // преобразуем в оттенки серого

    cv::Mat grad_x, grad_y; // в этих двух картинках мы получим производную (градиент=gradient) по оси x и y
    // для этого достаточно дважды применить оператор Собеля (реализованный в OpenCV)
    cv::Sobel(img, grad_x, CV_32FC1, 1, 0);
    cv::Sobel(img, grad_y, CV_32FC1, 0, 1);
    rassert(!grad_x.empty(), 234892748239070017);
    rassert(!grad_y.empty(), 234892748239070018);


    std::string resultsDir = "../../../../lesson07/resultsData/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }
    // сохраняем оба результата в картинку на диск, чтобы проверить что результат выглядит разумно
    //cv::imwrite(resultsDir + name + "_1_sobel_x.png", grad_x);
    //cv::imwrite(resultsDir + name + "_2_sobel_y.png", grad_y);

    // замечаем что мы ведь забыли взять абсолютное значение градиента!
    // TODO посмотрите на картинки на диске, какая из картинок это явно показывает?
    // давайте это исправим:
    grad_x = cv::abs(grad_x);
    grad_y = cv::abs(grad_y);
    //cv::imwrite(resultsDir + name + "_3_sobel_x.png", grad_x);
    //cv::imwrite(resultsDir + name + "_4_sobel_y.png", grad_y);
    // TODO посмотрите на картинки и проверьте поправилось ли?

    cv::Mat sobel_strength(img.rows, img.cols, CV_32FC1, 0.0f);
    // теперь хочется заполнить sobel_strength силой градиента с учетом обеих осей, т.е. что-то вроде sobel_strength=sqrt(grad_x^2+grad_x^2):
    for (int i = 0; i < sobel_strength.rows; i++) {
        for (int j = 0; j < sobel_strength.cols; j++) {
            // TODO реализуйте здесь заполнение sobel_strength с учетом grad_x, grad_y:
            float dx = grad_x.at<float>(i, j);
            float dy = grad_y.at<float>(i, j);
            float gradient_strength = sqrt(dx * dx + dy * dy); // TODO
            sobel_strength.at<float>(i, j) = gradient_strength;
        }
    }
    cv::imwrite(resultsDir + name + "_5_sobel_strength.png", sobel_strength);

    cv::Mat hough = buildHough(sobel_strength); // TODO теперь зайдите внутрь этой функции и реализуйте построение пространства Хафа

    //cv::imwrite(resultsDir + name + "_6_hough.png", hough);
    // обратите внимание что почти все пространство - яркое белое или черное, это происходит потому что яркость сильно больше чем 255

     //TODO поправьте это - найдите максимальную яркость (max_accumulated) среди всей матрицы hough и после этого отнормируйте всю картинку:
     float max_accumulated = 0.0f;
     for (int i = 0; i < hough.rows; i++) {
         for (int j = 0; j < hough.cols; j++) {
             if (max_accumulated < hough.at<float>(i, j)) max_accumulated = hough.at<float>(i, j);
         }
     }
     //TODO замените каждый пиксель с яркости X на яркость X*255/max_accumulated (т.е. уменьшите диапазон значений):
     for (int i = 0; i < hough.rows; i++) {
         for (int j = 0; j < hough.cols; j++) {
             hough.at<float>(i, j) = hough.at<float>(i, j) * 255 / max_accumulated;
         }
     }
     cv::imwrite(resultsDir + name + "_7_hough_normalized.png", hough);
}


int main() {
    try {
        // TODO посмотрите на результат (аккумулятор-пространство Хафа) на всех этих картинках (раскомментируя их одну за другой)
        // TODO подумайте и напишите здесь оветы на вопросы:
        test("line01");
        // 1) Какие координаты примерно должны бы быть у самой яркой точки в картинке line01_7_hough_normalized.png?
        // ответ:

        test("line02");
        // 2) Какие координаты примерно должны бы быть у самой яркой точки в картинке line02_7_hough_normalized.png?
        // ответ:

        test("line11");
        // 3) Чем должно бы принципиально отличаться пространство Хафа относительно случая line01?
        // ответ:

        test("line12");
        // 4) Зная правильный ответ из предыдущего случая line11 - как найти правильнйы ответ для line12?
        // ответ:

        test("line21_water_horizont");
        // 5) Сколько должно бы быть ярких точек?
        // ответ:

        test("multiline1_paper_on_table");
        // 6) Сколько должно бы быть ярких точек? Сколько вы насчитали в пространстве Хафа?
        // ответ:

        test("multiline2_paper_on_table");
        // 7) Сколько должно бы быть ярких точек? Сколько вы насчитали в пространстве Хафа? Есть ли интересные наблюдения относительно предыдущего случая?
        // ответ:

        test("valve");
        // 8) Какие-нибудь мысли?

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}

