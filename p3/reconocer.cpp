#include "opencv2/opencv.hpp"
#include "iostream"
#include "functions.h"
#include "Shape.h"
#include <string>
#include <map>

using namespace std;

cv::RNG rng(12345);
double chi_2_05 = 5.99;

void recognize(cv::Mat, Shape*);
Shape* loadObjects();
double mahalanobis(vector<cv::Point>, Shape&);

int main(int argc, char* argv[])
{
    
    if (argc != 2)
    {
        cout << "Usage: ./reconocer <nomFich>" << endl;
        return -1;
    }

    string file = argv[1];
    
    cv::Mat img = imread(file, cv::IMREAD_GRAYSCALE);
    if (img.empty())
    {
        cout << "ERROR reading image: " << file << endl;
    }
    imshow("xd", img);
    cv::waitKey(0);

    Shape* objects = loadObjects();

    recognize(img, objects);
}

void recognize(cv::Mat img, Shape* objects)
{
    cv::Mat res = threshold(img);
    vector<vector<cv::Point>> contours = getBlobs(res);

    for (int i = 0; i < (int)contours.size(); i++)
    {
        vector<string> result;
        string object_detected = "unknown";
        double min = 10000;

        for (int j = 0; j < 5; j++)
        {
            Shape f = objects[j];

            double m = mahalanobis(contours.at(i), f);
            if (m < chi_2_05)
            {
                result.push_back(f.name);
                if (m < min)
                {
                    min = m;
                    object_detected = f.name;
                }
            }
        }
        if (result.size() == 0) cout << "Unrecognized object" << endl;
        else
        {
            cout << "Predicted object: " << object_detected << endl;
            if (result.size() > 1)
            {
                for (int j = 0; j < (int)result.size(); j++)
                {
                    cout << " -" << result.at(j) << endl;
                }
            }
        }
        vector<cv::Vec4i> hierarchy;
        cv::Mat drawing = cv::Mat::zeros(img.size(), CV_8UC3);
        cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        drawContours(drawing, contours, i, color, cv::FILLED, 0, hierarchy, 0, cv::Point());
        imshow("Object: " + object_detected + "_" + to_string(i), drawing);
        cv::waitKey(0);
    }
}

Shape* loadObjects()
{
    Shape circulo = Shape("circulo");
    Shape rectangulo = Shape("rectangulo");
    Shape rueda = Shape("rueda");
    Shape triangulo = Shape("triangulo");
    Shape vagon = Shape("vagon");

    cv::FileStorage fs_read("objects.yml", cv::FileStorage::READ);
    if (fs_read.isOpened())
    {
        circulo.load(fs_read);
        rectangulo.load(fs_read);
        rueda.load(fs_read);
        triangulo.load(fs_read);
        vagon.load(fs_read);
    }
    fs_read.release();

    static Shape objects[5] = { circulo, rectangulo, rueda, triangulo, vagon };

    return objects;
}

double mahalanobis(vector<cv::Point> x, Shape& s)
{
    double perimeter = arcLength(x, true);

    cv::Moments m = moments(x, true);
    double inv[7];
    HuMoments(m, inv);
    double area = m.m00;
    double inv1 = inv[1];
    double inv2 = inv[2];
    double inv3 = inv[3];

    double d = pow((area - s.area_mean), 2) / ((double)s.area_var);
    d += pow((perimeter - s.perimeter_mean), 2) / ((double)s.perimeter_var);

    return d;
}