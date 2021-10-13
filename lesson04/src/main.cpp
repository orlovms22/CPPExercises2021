#include <iostream>
#include <filesystem> // это нам понадобится чтобы создать папку для результатов
#include <libutils/rasserts.h>

#include "disjoint_set.h"
#include "morphology.h"

#include <opencv2/highgui.hpp>

// TODO 100 реализуйте систему непересекающихся множеств - см. файлы disjoint_set.h и disjoint_set.cpp
// чтобы их потестировать - постарайтесь дописать сюда много разных интересных случаев:
void testingMyDisjointSets() {
    DisjointSet set(5);
    rassert(set.count_differents() == 5, 2378923791);
    for (int element = 0; element < 5; element++) {
        rassert(set.get_set(element) == element, 23892803643);
        rassert(set.get_set_size(element) == 1, 238928031);
    }
    set.union_sets(0, 1);
    set.union_sets(2, 3);
    set.union_sets(3, 4);
    rassert(set.get_set(1) == 0, "ghsbgfbslu");
    rassert(set.get_set(4) == 2, "gshiuhuihg");
    rassert(set.get_set_size(0) == 2, "ughsiuhghg");
    rassert(set.get_set_size(3) == 3, "gysygysgygs");
    rassert(set.count_differents() == 2, "gsuhliughiugh");
    set.union_sets(1, 3);
    std::cout << set.get_set(0) << std::endl;

    // TODO 100 по мере реализации DisjointSet - добавьте здесь каких-то вызовов операции "объединение двух множеств", сразу после этого проверяя через rassert что после этого результат такой как вы ожидаете
    // TODO 100 затем попробуйте создать СНМ размера 10.000.000 - и пообъединяйте какие-нибудь элементы (в цикле), быстро ли работает? а если при подвешивании одного корня множества к другому мы не будем учитывать ранк (высоту дерева) - как быстро будет работать?
    // TODO 100 попробуйте скомпилировать программу с оптимизациями и посмотреть ускорится ли программа - File->Settings->CMake->Плюсик над Debug->и переключите его в RelWithDebInfo (чтобы были хоть какие-то отладочные символы)
    std::cout << "start" << std::endl;
    DisjointSet large_set(10000000);
    for (int element = 1; element < 10000000-1; element++)
    {
        if (element < 5000000) large_set.union_sets(0, element);
        else large_set.union_sets(10000000 - 1, element);
    }
    std::cout << "stop" << std::endl;
}

// TODO 200 перенесите сюда основную часть кода из прошлого задания про вычитание фона по первому кадру, но:
// 1) добавьте сохранение на диск визуализации картинок:
// 1.1) картинка эталонного фона
// 1.2) картинка текущего кадра
// 1.3) картинка визуализирующая маску "похож ли пиксель текущего кадра на эталонный фон"
// 1.4) картинка визуализирующая эту маску после применения волшебства морфологии
// 1.5) картинка визуализирующая эту маску после применения волшебства СНМ (системы непересекающихся множеств)
// 2) сохраняйте эти картинки визуализации только для тех кадров, когда пользователем был нажат пробел (код 32)
// 3) попробуйте добавить с помощью нажатия каких то двух кнопок "усиление/ослабление подавления фона"
// 4) попробуйте поменять местами морфологию и СНМ
// 5) попробуйте добавить настройку параметров морфологии и СНМ по нажатию кнопок (и выводите их значения в консоль)
void backgroundMagickStreaming() {
    cv::VideoCapture video(0);
    rassert(video.isOpened(), 3423948392481); // проверяем что видео получилось открыть

    cv::Mat frame;
    cv::Mat preImage;
    bool ON = false;

    cv::Mat largeCastle;
    largeCastle = cv::imread("../../../../lesson03/data/castle_large.jpg");

    std::string resultsDir = "../../../../lesson04/resultsData/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }

    while (video.isOpened()) { // пока видео не закрылось - бежим по нему
        bool isSuccess = video.read(frame); // считываем из видео очередной кадр
        rassert(isSuccess, 348792347819); // проверяем что считывание прошло успешно
        rassert(!frame.empty(), 3452314124643); // проверяем что кадр не пустой

        int key = cv::waitKey(10);
        if (ON)
        {
            cv::Mat mask = makeMask(frame.clone(), preImage.clone());
            if (key == 32)
            {
                std::string filename = resultsDir + "01_frame.jpg";
                cv::imwrite(filename, frame);
                filename = resultsDir + "02_mask.jpg";
                cv::imwrite(filename, mask);
            }
            mask = maskMorphology(mask.clone(), 3, 15);
            if (key == 32)
            {
                std::string filename = resultsDir + "03_mask_morphology.jpg";
                cv::imwrite(filename, mask);
            }
            mask = maskDisjointSet(mask.clone(), 12000);
            if (key == 32)
            {
                std::string filename = resultsDir + "04_mask_disjoint_set.jpg";
                cv::imwrite(filename, mask);
            }
            frame = fillBackground(frame.clone(), largeCastle.clone(), mask.clone());
            if (key == 32)
            {
                std::string filename = resultsDir + "05_result_image.jpg";
                cv::imwrite(filename, frame);
            }
        }

        cv::imshow("video", frame); // покаызваем очередной кадр в окошке

        if (key == 27) exit(0);
        if (key == 'q')
        {
            ON = !ON;
            preImage = frame.clone();
        }
    }
}

int main() {
    try {
        //testingMyDisjointSets();
        backgroundMagickStreaming();
        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
