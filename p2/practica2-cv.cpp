#include "opencv2/opencv.hpp"
#include "iostream"
#include <string>

using namespace std;

#define PI 3.14159265
#define SOBEL_THRESHOLD 60
#define CANNY_THRESHOLD 8

void sobel(cv::Mat, bool, bool);
void canny(cv::Mat, bool, bool);
void drawEdges(cv::Mat, int, bool, string);
void featurePoints(cv::Mat, cv::Mat, cv::Mat, int);
void draw_cross(int, cv::Mat);

float gaussiana(int x, float sigma) {
    return exp(-x * x / (2 * sigma * sigma));
}

float derivGaussiana(int x, float sigma) {
    float i = -x / (sigma * sigma);
    return i * exp((-(x * x)) / (2 * sigma * sigma));
}



int main() {
    cv::Mat imgPoster = cv::imread("poster.pgm", -1);
    if (imgPoster.empty()) {
        cout << "ERROR: Could not load the image poster" << endl;
        exit(1);
    }

    cv::Mat imgPas1 = cv::imread("pasillo1.pgm", -1);
    if (imgPas1.empty()) {
        cout << "ERROR: Could not load the image pasillo1" << endl;
        exit(1);
    }

    cv::Mat imgPas2 = cv::imread("pasillo2.pgm", -1);
    if (imgPas2.empty()) {
        cout << "ERROR: Could not load the image pasillo2" << endl;
        exit(1);
    }

    // 1 - Calculo del gradiente horizontal y vertical, modulo y orientacion del gradiente
    // Operadores de Sobel(filtrado Gaussiano con sigma variable)
    sobel(imgPoster.clone(), true, false);
    // Operadores de Canny (con sigma variable)
    canny(imgPoster.clone(), true, false);

    // 2 - Deteccion punto central de un pasillo (transformada de Hough)
    sobel(imgPas1.clone(), false, true);
    sobel(imgPas2.clone(), false, true);

	return 0;
}



void sobel(cv::Mat img, bool show, bool fp) {
    cv::Mat original_img = img.clone();

    int sigma = 3;  // convolution mask (kernel) size

    img.convertTo(img, CV_32F);

    cv::GaussianBlur(img, img, cv::Size(sigma, sigma), 0, 0, cv::BORDER_DEFAULT); 

    cv::Mat gx, gy, mag, ori, edges;

    cv::Sobel(img, gx, CV_32F, 1, 0, sigma);        // Gradient x
    cv::Sobel(img, gy, CV_32F, 0, 1, sigma);        // Gradient y
    gy = -gy;

    cv::Mat final_gx = (gx / 2 + 128) / 255;
    cv::Mat final_gy = (gy / 2 + 128) / 255;

    cartToPolar(gx, gy, mag, ori, false);           // Magnitude and Orientation
    cv::Mat final_mag = mag / 255;
    cv::Mat final_ori = (ori / (2 * PI));
   
    if (show) {
        cv::imshow("Gradient in x direction: Gx (Sobel)", final_gx);
        cv::imshow("Gradient in y direction: Gy (Sobel)", final_gy);
        cv::imshow("Gradient magnitude (Sobel)", final_mag);
        cv::imshow("Gradient orientation (Sobel)", final_ori);
    }

    drawEdges(mag, SOBEL_THRESHOLD, show, "Sobel");    // Edges

    if (show) cv::waitKey(0);

    if (fp) featurePoints(original_img, mag, ori, SOBEL_THRESHOLD);          // Feature Point
}


void canny(cv::Mat img, bool show, bool fp) {
    cv::Mat original_img = img.clone();

    int sigma = 1;

    img.convertTo(img, CV_32F);

    cv::Mat gx(cv::Size(img.cols, img.rows), CV_32F);
    cv::Mat gy(cv::Size(img.cols, img.rows), CV_32F);

    int mask_size = 5 * sigma;
    if (mask_size % 2 == 0) mask_size++;

    cv::Mat gX(cv::Size(mask_size, 1), CV_32F);
    cv::Mat gY(cv::Size(1, mask_size), CV_32F);
    cv::Mat gXd(cv::Size(mask_size, 1), CV_32F);
    cv::Mat gYd(cv::Size(1, mask_size), CV_32F);

    float x, xDeriv, y, yDeriv;

    // Gradient x
    float sumDeriv = 0.0, sumGauss = 0.0;
    for (int i = 0; i < mask_size; i++) { 
        x = gaussiana(i - 2, sigma);
        if (x > 0.0) sumGauss += x;
        gY.at<float>(i, 0) = x;

        xDeriv = derivGaussiana(i - 2, sigma);
        if (xDeriv > 0) sumDeriv += xDeriv;
        gXd.at<float>(0, i) = xDeriv;
    }

    float k = sumGauss * sumDeriv;
    cv::Mat kernel(cv::Size(1, 1), CV_32F);
    kernel = gY * gXd;
    filter2D(img, gx, -1, kernel);

    gx = -gx;
    gx = gx / k;
    cv::Mat final_gx = (gx + 128) / 255;

    // Gradient y
    sumDeriv = 0.0, sumGauss = 0.0;
    for (int i = 0; i < mask_size; i++) {
        y = gaussiana(i - 2, sigma);
        if (y > 0.0) sumGauss += y;
        gX.at<float>(0, i) = y;

        yDeriv = derivGaussiana(i - 2, sigma);
        if (yDeriv > 0) sumDeriv += yDeriv;
        gYd.at<float>(i, 0) = yDeriv;
    }
    k = sumGauss * sumDeriv;
    kernel = gYd * gX;
    filter2D(img, gy, -1, kernel);
    gy = gy / k;
    cv::Mat final_gy = (gy + 128) / 255;

    // Magnitude and Orientation
    cv::Mat mag, ori;
    cv::cartToPolar(gx, gy, mag, ori, false);
    cv::Mat final_mag = mag / 255;
    cv::Mat final_ori = (ori / (2 * PI));

    if (show) {
        cv::imshow("Gradient in x direction: Gx (Canny)", final_gx);
        cv::imshow("gradY Canny", final_gy);
        cv::imshow("Modulo Canny", final_mag);
        cv::imshow("Orientacion Canny", final_ori);
    }

    drawEdges(mag, CANNY_THRESHOLD, show, "Canny");    // Edges

    if(show) cv::waitKey(0);

    if (fp) featurePoints(original_img, mag, ori, CANNY_THRESHOLD);          // Feature point
}


void drawEdges(cv::Mat mag, int threshold, bool show, string type) {
    cv::Mat final = mag.clone();
    for (int i = 0; i < mag.rows; i++) {
        for (int j = 0; j < mag.cols; j++) {
            if (mag.at<float>(i, j) > threshold) {
                circle(final, cv::Point(j, i), 1, CV_RGB(255, 255, 255));
            }
        }
    }
    // Muestra los puntos con modulo mayor que el umbral
    if (show) {
        cv::imshow("Result after gradient maxima : Edges (" + type + ")", final / 255);
    }
}


void featurePoints(cv::Mat img, cv::Mat mag, cv::Mat ori, int threshold) {
    vector<int> votes(50);
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            if (mag.at<float>(i, j) > threshold) {
                float th = ori.at<float>(i, j);
                float dist = cosf(th);
                if (dist < 0) dist = -dist;
                if (dist < 0.9) {
                    int x = j - img.cols / 2;
                    int y = img.rows / 2 - i;
                    float rho = x * cosf(th) + y * sinf(th);
                    int vote = static_cast<int>(rho / cosf(th));
                    vote = (vote + mag.cols / 2) / 10;
                    if (vote < votes.size() && vote >= 0) votes[vote] += 1;
                }
            }
        }
    }
    int max = 0;
    int index = 0;

    max = 0;
    index = 0;
    for (unsigned int i = 0; i < votes.size(); i++) {
        int vote = votes.at(i);
        if (vote > max) {
            max = vote;
            index = i * 10;
        }
    }

    draw_cross(index, img);

    cv::waitKey(0);
}


void draw_cross(int x, cv::Mat mat) {
    int y = mat.rows / 2;
    cv::Point p1 = cv::Point(x, y - 5);
    cv::Point p2 = cv::Point(x, y + 5);
    cv::Point p3 = cv::Point(x - 5, y);
    cv::Point p4 = cv::Point(x + 5, y);

    line(mat, p1, p2, CV_RGB(255, 0, 0), 2);
    line(mat, p3, p4, CV_RGB(255, 0, 0), 2);
    circle(mat, cv::Point(x, y), 1, CV_RGB(255, 0, 0), 1);
    imshow("Feature poing", mat);
}