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
    // ���� ������ - �������� SIFT (�������������� � �������� �������� �����)
    cv::Ptr<cv::FeatureDetector> detector = cv::SIFT::create();

    // ����������� � ��������� �������� �����
    std::vector<cv::KeyPoint> keypoints0, keypoints1; // ����� ����� ��������� ������ �������� �����
    cv::Mat descriptors0, descriptors1; // ����� ����� ��������� ����������� ���� �������� �����
    std::cout << "Detecting SIFT keypoints and describing them (computing their descriptors)..." << std::endl;
    detector->detectAndCompute(img0, cv::noArray(), keypoints0, descriptors0);
    detector->detectAndCompute(img1, cv::noArray(), keypoints1, descriptors1);
    std::cout << "SIFT keypoints detected and described: " << keypoints0.size() << " and " << keypoints1.size() << std::endl; // TODO

    {
        // ������� �������� �� �������� ��� ��� ����� ���� ���������� ��� ������������
        cv::Mat img0withKeypoints, img1withKeypoints;
        cv::drawKeypoints(img0, keypoints0, img0withKeypoints);
        cv::drawKeypoints(img1, keypoints1, img1withKeypoints);
        cv::imwrite(results_path + "01keypoints0.jpg", img0withKeypoints);
        cv::imwrite(results_path + "01keypoints1.jpg", img1withKeypoints);
    }

    // ������ ������� ���������� �������� ����� ����� ��������� 0 � ��������� 1:
    // ����� ��� ������ ����� �� ������ �������� - ��� ����� ������� ����� �� ������ ��������
    std::vector<std::vector<cv::DMatch>> matches01;
    std::cout << "Matching " << keypoints0.size() << " points with " << keypoints1.size() << "..." << std::endl; // TODO
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    matcher->knnMatch(descriptors0, descriptors1, matches01, 2); // k: 2 - ��������� ��� �� ���� ��� ��������� �����, � �� ���� ����� ���������
    std::cout << "matching done" << std::endl;
    // �.�. �� ��� ������ ����� keypoints0 ���� ��������� �� keypoints1, �� ������������� ������� ������� �� ������� ����� � keypoints0:
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
        // ������� �������� ��� �������� ������������� ����� ������� (k - ��������� �� ����� ������������� �� ������ �������, �� ���������, ��� �� ������ �� ��������)
        cv::Mat imgWithMatches;
        cv::drawMatches(img0, keypoints0, img1, keypoints1, matchesK, imgWithMatches);
        cv::imwrite(results_path + "02matches01_k" + std::to_string(k) + ".jpg", imgWithMatches);
    }

    // ������ ������� ���������� �������� ����� ����� ��������� 1 � ��������� 0 (�.�. � �������� �������):
    std::vector<std::vector<cv::DMatch>> matches10;
    std::cout << "Matching " << keypoints1.size() << " points with " << keypoints0.size() << "..." << std::endl;
    matcher->knnMatch(descriptors1, descriptors0, matches10, 2); // k: 2 - ��������� ��� �� ���� ��� ��������� �����, � �� ���� ����� ���������
    std::cout << "matching done" << std::endl;

    // ������ ������� ��������� ������ ��������� �������������
    std::cout << "Filtering matches..." << std::endl;
    std::vector<cv::Point2f> points0, points1; // ����� �������� ���������� �������� ����� ��� �������� �����
    std::vector<unsigned char> matchIsGood; // ����� �� ������� true - ������� �������������, � false - ������
    int nmatchesGood = 0; // ��������� ������� ������������� ����������� ��������
    for (int i = 0; i < keypoints0.size(); ++i) {
        cv::DMatch match = matches01[i][0];
        rassert(match.queryIdx == i, 234782749278097); // � ����� - queryIdx ��� ������ ����� (������� ������ == i)
        int j = match.trainIdx; // � trainIdx - ��� ����� ����� �� ������� ������� ����� ��������� � ��� (� queryIdx �� ������� ������� �����) ���������
        rassert(j < keypoints1.size(), 38472957238099); // ������� ���� ��������� ��� ������ �� ����� �� ������� ������� ������� �����

        points0.push_back(keypoints0[i].pt);
        points1.push_back(keypoints1[j].pt);

        bool isOk = true;

        // TODO �������� left-right check, �.�. �������� ������ �� ���� ��� ����� � ����� ������� ��������� ����� �, �� �������� ��� �������� ������������� � � ����� � - ��������� �������� ����� �
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
    // TODO �������� ������� �� ������� ������������ �������� ����� ����������:
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
        // ������������� ������� �������������
        cv::drawMatches(img0, keypoints0, img1, keypoints1, goodMatches, imgWithMatches);
        cv::imwrite(results_path + "04goodMatches01.jpg", imgWithMatches);
    }

    // ������ �� ���� ���������� ������� ������������� - ������������� ������� ������� RANSAC � OpenCV ����� ����� ������� �������������� �� ������ �������� �� ������
    std::cout << "Finding Homography matrix from image0 to image1..." << std::endl;
    const double ransacReprojThreshold = 3.0;
    std::vector<cv::Point2f> pointsGood0 = filterPoints(points0, matchIsGood);
    std::vector<cv::Point2f> pointsGood1 = filterPoints(points1, matchIsGood);
    std::vector<unsigned char> inliersMask; // � ���� ������ RANSAC ������� ������ - ����� ������������� �� �������� ����������� (inliers)
    // RANSAC:
    cv::Mat H01 = cv::findHomography(pointsGood0, pointsGood1, cv::RANSAC, ransacReprojThreshold, inliersMask);
    // H01 - ������� 3�3 ����������� �������������� ��������� ������ �������� � ��������� ������ ��������
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

        // ������������� inliersMask - �.�. ����� ������������� � �������� ����� RANSAC �������� ����������� (�.�. �� ���������, �� outliers)
        std::cout << "(with " << ninliers << "/" << nmatchesGood << " inliers matches)" << std::endl;
        cv::Mat imgWithInliersMatches;
        cv::drawMatches(img0, inlierKeypoints0, img1, inlierKeypoints1, inliersMatches, imgWithInliersMatches);
        cv::imwrite(results_path + "05inliersMatches01.jpg", imgWithInliersMatches);
    }

    cv::Mat H10 = H01.inv(); // � ������� ���� �������� ������� - ������� ��, ����� �������������� ��� ������?
    cv::Mat img1to0(img0.size() + img1.size(), CV_8UC3); // ������� ������ ������ �������� �������� �� ������ � ������������ ������ �������� - �� ������ ���!
    for (int i = 0; i < img0.rows; i++) {
        for (int j = 0; j < img0.cols; j++) {
            img1to0.at<cv::Vec3b>(i, j) = img0.at<cv::Vec3b>(i, j);
        }
    }
    cv::warpPerspective(img1, img1to0, H10, img1to0.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);
    cv::imwrite(results_path + "06img0with1to0.jpg", img1to0);

    //return img1to0;
}
