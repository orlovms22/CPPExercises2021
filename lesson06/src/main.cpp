#include <filesystem>
#include <vector>
#include <iostream>
#include <libutils/rasserts.h>

#include "blur.h" // TODO реализуйте функцию блюра с произвольной силой размытия в файле blur.cpp

void testSomeBlur() {
    // TODO выберите любую картинку и любую силу сглаживания - проверьте что результат - чуть размытая картинка
    // Входные картинки для тестирования возьмите из предыдущего урока (т.е. по пути lesson05/data/*).
    // Результирующие картинки сохарняйте в эту папку (т.е. по пути lesson06/resultsData/*).
    std::string name = "valve";
    cv::Mat img = cv::imread("../../../../lesson05/data/" + name + ".jpg");
    rassert(!img.empty(), 23981920813);

    cv::Mat gaussian = blur(img, 10);

    std::string resultsDir = "../../../../lesson06/resultsData/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }

    cv::imwrite(resultsDir + name + "_blur.jpg", gaussian);
}

void testManySigmas() {
    // TODO возьмите ту же самую картинку но теперь в цикле проведите сглаживание для нескольких разных сигм
    // при этом результирующую картинку сохраняйте с указанием какая сигма использовалась:
    // для того чтобы в название файла добавить значение этой переменной -
    // воспользуйтесь функцией преобразующей числа в строчки - std::to_string(sigma)
    std::string resultsDir = "../../../../lesson06/resultsData/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }

    for (double sigma = 0.8; sigma < 10; sigma += 0.4)
    {
        std::string name = "valve";
        cv::Mat img = cv::imread("../../../../lesson05/data/" + name + ".jpg");
        rassert(!img.empty(), 23981920813);

        cv::Mat gaussian = blur(img, sigma);

        cv::imwrite(resultsDir + name + "_blur_sigma=" + std::to_string(sigma) + ".jpg", gaussian);
    }
}

int main() {
    try {
        testSomeBlur();
        testManySigmas();

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}

