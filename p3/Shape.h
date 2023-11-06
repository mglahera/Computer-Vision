#include "opencv2/opencv.hpp"
#include "iostream"

#ifndef SHAPE_H
#define SHAPE_H

using namespace std;

class Shape {
public:
    int n;
    string name;
    double area_mean;
    double area_var;
    double perimeter_mean;
    double perimeter_var;
    double m1_mean;
    double m1_var;
    double m2_mean;
    double m2_var;
    double m3_mean;
    double m3_var;

    Shape(string shapeName);
    void load(const cv::FileStorage& fs);
    void save(cv::FileStorage& fs);
    void update(double area, double perimeter, double m1, double m2, double m3);

    friend ostream& operator<<(ostream& os, const Shape& s); 
};

#endif 
