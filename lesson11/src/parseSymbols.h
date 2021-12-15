#pragma once

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <vector>

std::vector<cv::Mat> splitSymbols(cv::Mat img);

// ����� �� ����� ������� ��������� ������ ���������:
#define NBINS 16 // ����� ������ (�.�. ���������� ������ ����������� ���� ������� �� �������������)


typedef std::vector<double> HoG;

// TODO ��� ������� ��� ���� �����������
HoG buildHoG(cv::Mat grad_x, cv::Mat grad_y); // ��������� �� ���� ��������� �� ����� ����

HoG buildHoG(cv::Mat img); // ��������� �� ���� ������������ �������� � �������� ������� buildHoG ����������� ����

// TODO ��� ������� ��� ���� �����������
std::ostream& operator << (std::ostream& os, const HoG& hog);

// TODO ��� ������� ��� ���� �����������
double distance(HoG a, HoG b);
