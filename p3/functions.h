#include "opencv2/opencv.hpp"
#include "iostream"
#include "Shape.h"


#ifndef FUNCTIONS_H
#define FUNCTIONS_H

using namespace std;


cv::Mat threshold(cv::Mat img)
{
	cv::Mat res, outAdap;

    // Adaptive method
	//cv::adaptiveThreshold(img, outAdap, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 80, 2);
	//cv::adaptiveThreshold(img, outAdap, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 140, 2);

    // Otsu method
	cv::threshold(img, res, 0, 255, cv::THRESH_BINARY_INV|cv::THRESH_OTSU);

	imshow("Output with Otsu", res);
	cv::waitKey(0);

	return res;
}

vector<vector<cv::Point>> getBlobs(cv::Mat img)
{
    vector<vector<cv::Point>> contours, aux;
    vector<cv::Vec4i> hierarchy;    // Not used

    // Find contours
    cv::findContours(img, aux, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    vector<cv::Point> max = aux[0];
    for (const auto& contour : aux)
    {
        if (contourArea(contour) > 10)
        {
            contours.push_back(contour);
        }
    }
    return contours;
}

void getDescriptors(vector<vector<cv::Point>> contours, int obj)
{
    double p = arcLength(contours.at(0), true);
    cv::Moments m = moments(contours[0], true);
    double inv[7];
    HuMoments(m, inv);
    double a = contourArea(contours[0]);
    double m1 = inv[0];
    double m2 = inv[1];
    double m3 = inv[2];

    // Write descriptors
    Shape rectangulo = Shape("rectangulo");
    Shape rueda = Shape("rueda");
    Shape triangulo = Shape("triangulo");
    Shape vagon = Shape("vagon");
    Shape circulo = Shape("circulo");

    cv::FileStorage fs_read("objetos.yml", cv::FileStorage::READ);
    if (fs_read.isOpened())
    {
        rectangulo.load(fs_read);
        rueda.load(fs_read);
        triangulo.load(fs_read);
        vagon.load(fs_read);
        circulo.load(fs_read);
    }
    fs_read.release();

    switch (obj)
    {
    case 0:
        circulo.update(a, p, m1, m2, m3);
        break;
    case 1:
        rectangulo.update(a, p, m1, m2, m3);
        break;
    case 2:
        rueda.update(a, p, m1, m2, m3);
        break;
    case 3:
        triangulo.update(a, p, m1, m2, m3);
        break;
    case 4:
        vagon.update(a, p, m1, m2, m3);
        break;
    default:
        cout << "ERROR: Incorrect name of object" << endl;
        break;
    }

    cv::FileStorage fs_write("objects.yml", cv::FileStorage::WRITE);
    circulo.save(fs_write);
    rectangulo.save(fs_write);
    rueda.save(fs_write);
    triangulo.save(fs_write);
    vagon.save(fs_write);

    fs_write.release();
}

#endif
