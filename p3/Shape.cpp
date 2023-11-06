#include "opencv2/opencv.hpp"
#include "Shape.h"

const double alfa = 0.1;

Shape::Shape(string shapeName)
{
    name = shapeName;
    n = 0;
    area_mean = 0;
    area_var = 0;
    perimeter_mean = 0;
    perimeter_var = 0;
    m1_mean = 0;
    m1_var = 0;
    m2_mean = 0;
    m2_var = 0;
    m3_mean = 0;
    m3_var = 0;
}

void Shape::load(const cv::FileStorage& fs)
{
    cv::FileNode fn = fs[name];
    n = fn["n"];
    area_mean = fn["Area-Mean"];
    area_var = fn["Area-Variance"];
    perimeter_mean = fn["Perimetro-Mean"];
    perimeter_var = fn["Perimetro-Variance"];
    m1_mean = fn["MI1-Mean"];
    m1_var = fn["MI1-Variance"];
    m2_mean = fn["MI2-Mean"];
    m2_var = fn["MI2-Variance"];
    m3_mean = fn["MI3-Mean"];
    m3_var = fn["MI3-Variance"];
}

void Shape::save(cv::FileStorage& fs)
{
    fs << name << "{";
    fs << "n" << n;
    fs << "Area-Mean" << area_mean << "Area-Variance" << area_var;
    fs << "Perimetro-Mean" << perimeter_mean << "Perimetro-Variance" << perimeter_var;
    fs << "MI1-Mean" << m1_mean << "MI1-Variance" << m1_var;
    fs << "MI2-Mean" << m2_mean << "MI2-Variance" << m2_var;
    fs << "MI3-Mean" << m3_mean << "MI3-Variance" << m3_var;
    fs << "}";
}

void Shape::update(double area, double perimeter, double m1, double m2, double m3)
{
    // AREA
    double new_area_mean = (((area_mean * n) + area)) / (n + 1);
    double prioriVar = pow(new_area_mean * alfa, 2);
    double new_area_var;
    if (n == 0) new_area_var = prioriVar;
    else
    {
        double normalVar = (((n - 1) * area_var) + ((area - area_mean) * (area - (new_area_mean)))) / (n);
        new_area_var = (prioriVar / (n + 1)) + (((n) / (n + 1.0)) * normalVar);
    }
    area_mean = new_area_mean;
    area_var = new_area_var;

    // PERIMETER
    double new_perimeter_mean = (((perimeter_mean * n) + perimeter)) / (n + 1);

    prioriVar = pow(new_perimeter_mean * alfa, 2);
    double new_perimeter_var;
    if (n == 0) new_perimeter_var = prioriVar;
    else
    {
        double normalVar =
            (((n - 1) * perimeter_var) + ((perimeter - perimeter_mean) * (perimeter - (new_perimeter_mean)))) / (n);
        new_perimeter_var = (prioriVar / (n + 1)) + (((n) / (n + 1.0)) * normalVar);
    }
    perimeter_mean = new_perimeter_mean;
    perimeter_var = new_perimeter_var;
   
    // M1
    double new_m1_mean = (((m1_mean * n) + m1)) / (n + 1);

    prioriVar = pow(new_m1_mean * alfa, 2);
    double new_m1_var;
    if (n == 0) new_m1_var = prioriVar;

    else
    {
        double normalVar = (((n - 1) * m1_var) + ((m1 - m1_mean) * (m1 - (new_m1_mean)))) / (n);
        new_m1_var = (prioriVar / (n + 1)) + (((n) / (n + 1.0)) * normalVar);
    }
    m1_mean = new_m1_mean;
    m1_var = new_m1_var;

    // M2
    double new_m2_mean = (((m2_mean * n) + m2)) / (n + 1);

    prioriVar = pow(new_m2_mean * alfa, 2);
    double new_m2_var;
    if (n == 0) new_m2_var = prioriVar;
    else
    {
        double normalVar = (((n - 1) * m2_var) + ((m2 - m2_mean) * (m2 - (new_m2_mean)))) / (n);
        new_m2_var = (prioriVar / (n + 1)) + (((n) / (n + 1.0)) * normalVar);
    }
    m2_mean = new_m2_mean;
    m2_var = new_m2_var;

    // M3
    double new_m3_mean = (((m3_mean * n) + m3)) / (n + 1);

    prioriVar = pow(new_m3_mean * alfa, 2);
    double new_m3_var;
    if (n == 0) new_m3_var = prioriVar;
    else
    {
        double normalVar = (((n - 1) * m3_var) + ((m3 - m3_mean) * (m3 - (new_m3_mean)))) / (n);
        new_m3_var = (prioriVar / (n + 1)) + (((n) / (n + 1.0)) * normalVar);
    }
    m3_mean = new_m3_mean;
    m3_var = new_m3_var;

    n++;
}

ostream& operator<<(ostream& os, const Shape& s)
{
    os << s.name << " {" << endl;
    os << " - N: " << s.n << endl;
    os << " - Area-Mean: " << s.area_mean << ", Area-Variance: " << s.area_var << endl;
    os << " - Perimetro-Mean: " << s.perimeter_mean << ", Perimetro-Variance: " << s.perimeter_var << endl;
    os << " - MI1-Mean: " << s.m1_mean << ", MI1-Variance: " << s.m1_var << endl;
    os << " - MI2-Mean: " << s.m2_mean << ", MI2-Variance: " << s.m2_var << endl;
    os << " - MI3-Mean: " << s.m3_mean << ", MI3-Variance: " << s.m3_var << endl;
    os << "}";
    return os;
}



