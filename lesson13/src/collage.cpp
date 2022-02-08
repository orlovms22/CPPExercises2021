#include "collage.h"

std::vector<cv::Point2f> filterPoints(std::vector<cv::Point2f> points, std::vector<unsigned char> matchIsGood) {
    rassert(points.size() == matchIsGood.size(), 234827348927016);

    std::vector<cv::Point2f> goodPoints;
    for (int i = 0; i < matchIsGood.size(); ++i) {
        if (matchIsGood[i]) {
            goodPoints.push_back(points[i]);
        }
    }
    return goodPoints;
}

void collage(cv::Mat img0, cv::Mat img1, std::string results_path) {
    // Этот объект - алгоритм SIFT (детектирования и описания ключевых точек)
    cv::Ptr<cv::FeatureDetector> detector = cv::SIFT::create();

    // Детектируем и описываем ключевые точки
    std::vector<cv::KeyPoint> keypoints0, keypoints1; // здесь будет храниться список ключевых точек
    cv::Mat descriptors0, descriptors1; // здесь будут зраниться дескрипторы этих ключевых точек
    std::cout << "Detecting SIFT keypoints and describing them (computing their descriptors)..." << std::endl;
    detector->detectAndCompute(img0, cv::noArray(), keypoints0, descriptors0);
    detector->detectAndCompute(img1, cv::noArray(), keypoints1, descriptors1);
    std::cout << "SIFT keypoints detected and described: " << keypoints0.size() << " and " << keypoints1.size() << std::endl; // TODO

    {
        // Давайте нарисуем на картинке где эти точки были обнаружены для визуализации
        cv::Mat img0withKeypoints, img1withKeypoints;
        cv::drawKeypoints(img0, keypoints0, img0withKeypoints);
        cv::drawKeypoints(img1, keypoints1, img1withKeypoints);
        cv::imwrite(results_path + "01keypoints0.jpg", img0withKeypoints);
        cv::imwrite(results_path + "01keypoints1.jpg", img1withKeypoints);
    }

    // Теперь давайте сопоставим ключевые точки между картинкой 0 и картинкой 1:
    // найдя для каждой точки из первой картинки - ДВЕ самые похожие точки из второй картинки
    std::vector<std::vector<cv::DMatch>> matches01;
    std::cout << "Matching " << keypoints0.size() << " points with " << keypoints1.size() << "..." << std::endl; // TODO
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    matcher->knnMatch(descriptors0, descriptors1, matches01, 2); // k: 2 - указывает что мы ищем ДВЕ ближайшие точки, а не ОДНУ САМУЮ БЛИЖАЙШУЮ
    std::cout << "matching done" << std::endl;
    // т.к. мы для каждой точки keypoints0 ищем ближайшую из keypoints1, то сопоставлений найдено столько же сколько точек в keypoints0:
    rassert(keypoints0.size() == matches01.size(), 234728972980049);

    double median;
    {
        std::vector<double> distances;
        for (int i = 0; i < matches01.size(); ++i) {
            distances.push_back(matches01[i][0].distance);
        }
        std::sort(distances.begin(), distances.end()); // GOOGLE: "cpp how to sort vector"
        std::cout << "matches01 distances min/median/max: " << distances[0] << "/" << distances[distances.size() / 2] << "/" << distances[distances.size() - 1] << std::endl;
        median = distances[distances.size() / 2];
    }
    for (int k = 0; k < 2; ++k) {
        std::vector<cv::DMatch> matchesK;
        for (int i = 0; i < matches01.size(); ++i) {
            matchesK.push_back(matches01[i][k]);
        }
        // давайте взглянем как выглядят сопоставления между точками (k - указывает на какие сопоставления мы сейчас смотрим, на ближайшие, или на вторые по близости)
        cv::Mat imgWithMatches;
        cv::drawMatches(img0, keypoints0, img1, keypoints1, matchesK, imgWithMatches);
        cv::imwrite(results_path + "02matches01_k" + std::to_string(k) + ".jpg", imgWithMatches);
    }

    // Теперь давайте сопоставим ключевые точки между картинкой 1 и картинкой 0 (т.е. в обратную сторону):
    std::vector<std::vector<cv::DMatch>> matches10;
    std::cout << "Matching " << keypoints1.size() << " points with " << keypoints0.size() << "..." << std::endl;
    matcher->knnMatch(descriptors1, descriptors0, matches10, 2); // k: 2 - указывает что мы ищем ДВЕ ближайшие точки, а не ОДНУ САМУЮ БЛИЖАЙШУЮ
    std::cout << "matching done" << std::endl;

    // Теперь давайте попробуем убрать ошибочные сопоставления
    std::cout << "Filtering matches..." << std::endl;
    std::vector<cv::Point2f> points0, points1; // здесь сохраним координаты ключевых точек для удобства позже
    std::vector<unsigned char> matchIsGood; // здесь мы отметим true - хорошие сопоставления, и false - плохие
    int nmatchesGood = 0; // посчитаем сколько сопоставлений посчиталось хорошими
    for (int i = 0; i < keypoints0.size(); ++i) {
        cv::DMatch match = matches01[i][0];
        rassert(match.queryIdx == i, 234782749278097); // и вновь - queryIdx это откуда точки (поэтому всегда == i)
        int j = match.trainIdx; // и trainIdx - это какая точка из второго массива точек оказалась к нам (к queryIdx из первого массива точек) ближайшей
        rassert(j < keypoints1.size(), 38472957238099); // поэтому явно проверяем что индекс не вышел за пределы второго массива точек

        points0.push_back(keypoints0[i].pt);
        points1.push_back(keypoints1[j].pt);

        bool isOk = true;

        // TODO добавьте left-right check, т.е. проверку правда ли если для точки А самой похожей оказалась точка Б, то вероятно при обратном сопоставлении и у точки Б - ближайшей является точка А
        cv::DMatch match01 = match;
        cv::DMatch match10 = matches10[j][0];
        if (match01.queryIdx != match10.trainIdx || match10.queryIdx != match01.trainIdx) {
            isOk = false;
        }

        if (isOk) {
            ++nmatchesGood;
            matchIsGood.push_back(true);
        }
        else {
            matchIsGood.push_back(false);
        }
    }
    rassert(points0.size() == points1.size(), 3497282579850108);
    rassert(points0.size() == matchIsGood.size(), 3497282579850109);
    // TODO выведите сколько из скольки соответствий осталось после фильтрации:
    std::cout << nmatchesGood << "/" << keypoints0.size() << " good matches left" << std::endl;

    {
        std::vector<cv::DMatch> goodMatches;
        for (int i = 0; i < matches01.size(); ++i) {
            cv::DMatch match = matches01[i][0];
            rassert(match.queryIdx == i, 2347982739280182);
            //rassert(match.trainIdx < points1.size(), 2347982739280182);
            if (!matchIsGood[i])
                continue;

            goodMatches.push_back(match);
        }
        cv::Mat imgWithMatches;
        // визуализируем хорошие соопставления
        cv::drawMatches(img0, keypoints0, img1, keypoints1, goodMatches, imgWithMatches);
        cv::imwrite(results_path + "04goodMatches01.jpg", imgWithMatches);
    }

    // Теперь на базе оставшихся хороших сопоставлений - воспользуемся готовым методом RANSAC в OpenCV чтобы найти матрицу преобразования из первой картинки во вторую
    std::cout << "Finding Homography matrix from image0 to image1..." << std::endl;
    const double ransacReprojThreshold = 3.0;
    std::vector<cv::Point2f> pointsGood0 = filterPoints(points0, matchIsGood);
    std::vector<cv::Point2f> pointsGood1 = filterPoints(points1, matchIsGood);
    std::vector<unsigned char> inliersMask; // в этот вектор RANSAC запишет флажки - какие сопоставления он посчитал корректными (inliers)
    // RANSAC:
    cv::Mat H01 = cv::findHomography(pointsGood0, pointsGood1, cv::RANSAC, ransacReprojThreshold, inliersMask);
    // H01 - матрица 3х3 описывающая преобразование плоскости первой картинки в плоскость второй картинки
    std::cout << "homography matrix found:" << std::endl;
    std::cout << H01 << std::endl;
    {
        std::vector<cv::DMatch> inliersMatches;
        std::vector<cv::KeyPoint> inlierKeypoints0, inlierKeypoints1;

        int ninliers = 0;
        for (int i = 0; i < inliersMask.size(); ++i) {
            if (inliersMask[i]) {
                ++ninliers;
                cv::DMatch match;
                match.queryIdx = inlierKeypoints0.size();
                match.trainIdx = inlierKeypoints1.size();
                inliersMatches.push_back(match);
                inlierKeypoints0.push_back(cv::KeyPoint(pointsGood0[i], 3.0));
                inlierKeypoints1.push_back(cv::KeyPoint(pointsGood1[i], 3.0));
            }
        }

        // визуализируем inliersMask - т.е. какие сопоставления в конечном счете RANSAC посчитал корректными (т.е. не выбросами, не outliers)
        std::cout << "(with " << ninliers << "/" << nmatchesGood << " inliers matches)" << std::endl;
        cv::Mat imgWithInliersMatches;
        cv::drawMatches(img0, inlierKeypoints0, img1, inlierKeypoints1, inliersMatches, imgWithInliersMatches);
        cv::imwrite(results_path + "05inliersMatches01.jpg", imgWithInliersMatches);
    }

    cv::Mat H10 = H01.inv(); // у матрицы есть обратная матрица - находим ее, какое преобразование она делает?
    cv::Mat img1to0(img0.size() + img1.size(), CV_8UC3); // давайте теперь вторую картинку нарисуем не просто в пространстве первой картинки - но поверх нее!
    for (int i = 0; i < img0.rows; i++) {
        for (int j = 0; j < img0.cols; j++) {
            img1to0.at<cv::Vec3b>(i, j) = img0.at<cv::Vec3b>(i, j);
        }
    }
    cv::warpPerspective(img1, img1to0, H10, img1to0.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);
    cv::imwrite(results_path + "06img0with1to0.jpg", img1to0);

    //return img1to0;
}
