#include "opencv2/opencv.hpp"
#include "iostream"
#include <string>

using namespace std;


// Functions declaration
cv::Mat useImage();
cv::Mat useWebcam();
void showMenu();
void plotIntensity(cv::Mat, string);
void plot(cv::Mat, string);
void histograma(cv::Mat);
cv::Mat alienEffect(cv::Mat, int);
cv::Mat alienEffectAux(cv::Mat, int);
cv::Mat posterEffect(cv::Mat, int);
cv::Mat distortion(cv::Mat, double);


/*********************
 *  Main program 
 *********************/
int main(int, char**) {
    cout << "Use image  -> Press '1'" << endl;
    cout << "Use webcam -> Press '2'" << endl;
    cout << "> ";
    int option = 0;
    cin >> option;

    if (option != 1 && option != 2) exit(0);

    cv::Mat originalframe;
    if (option == 1) originalframe = useImage();
    else originalframe = useWebcam();

    while (1) {
        bool end = false, contrast = false;;
        cv::Mat frame_result, frame = originalframe.clone();

        showMenu();
        int effect = 0;
        double coeficient = 0.0;
        cin >> effect;

        switch (effect)
        {
        case(1):
            histograma(frame);
            contrast = true;
            break;
        case(2):
            cout << "Selecciona color de efecto (0 verde, 1 azul, 2 rojo)" << endl;
            int option;
            cin >> option;
            frame_result = alienEffect(frame, option);
            break;
        case(3):
            cout << "Selecciona el numero de coleres para la reduccion" << endl;
            int numColors;
            cin >> numColors;
            frame_result = posterEffect(frame, numColors);
            break;
        case(4):
            cout << "Introduce el valor del coeficiente de distorsion: " << endl;
            cin >> coeficient;
            frame_result = distortion(frame, coeficient);
            break;
        case(5):
            cout << "Introduce el valor del coeficiente de distorsion: " << endl;
            cin >> coeficient;
            frame_result = distortion(frame, -coeficient);
            break;
        case(6):
            cv::flip(frame, frame_result, 1);
            break;
        case(7):
            cv::cvtColor(frame, frame_result, cv::COLOR_RGB2GRAY);
            break;
        default:
            end = true;
            break;
        }
        if (end) break;

        if (frame_result.empty() && !contrast) {
            cout << "ERROR: Could not create the image" << endl;
            exit(1);
        }

        if (!contrast) {
            cv::namedWindow("Result", cv::WINDOW_AUTOSIZE);
            cv::imshow("Result", frame_result);
            cv::waitKey(0);
            cv::destroyWindow("Result");
        }
    }
    return 0;
}

cv::Mat useImage() {
    cout << "Itroduce image name:" << endl;
    cout << "> ";
    string nameFile = "";
    cin >> nameFile;
    cv::Mat img = cv::imread(nameFile, -1);
    if (img.empty()) {
        cout << "ERROR: Could not load the image" << endl;
        exit(1);
    }
    cv::namedWindow("Image", CV_WINDOW_AUTOSIZE);
    cv::imshow("Image", img);
    cv::waitKey(0);
    return img;
}


cv::Mat useWebcam() {
    // Open the first webcam plugged in the computer
    cv::VideoCapture camera(0);
    if (!camera.isOpened()) {
        cout << "ERROR: Could not open camera" << endl;
        exit(1);
    }
    cv::namedWindow("Webcam", CV_WINDOW_AUTOSIZE);
    cv::Mat frame;
    camera >> frame;    // Capture the next frame from the webcam
    cv::imshow("Webcam", frame);
    cv::waitKey(0);
    return frame;
}

void showMenu() {
    cout << "--------------------------------------------------------------" << endl;
    cout << "Aplicar contraste y equalizacion de historgrama -----> Press 1" << endl;
    cout << "Aplicar efecto de alien -----------------------------> Press 2" << endl;
    cout << "Aplicar efecto de poster ----------------------------> Press 3" << endl;
    cout << "Aplicar distorsion de barril ------------------------> Press 4" << endl;
    cout << "Aplicar distorsion de de cojin ----------------------> Press 5" << endl;
    cout << "Invertir imagen -------------------------------------> Press 6" << endl;
    cout << "Convertir imagen a blanco y negro -------------------> Press 7" << endl;
    cout << "(Press any key to close program)" << endl;
    cout << "--------------------------------------------------------------" << endl;
    cout << "> ";
}



void plotIntensity(cv::Mat src, string name) {
    cv::Mat dst, hist;

    /// Convert to grayscale
    cvtColor(src, src, cv::COLOR_BGR2GRAY);

    /// Establish the number of bins
    int histSize = 256;

    /// Set the ranges ( for B,G,R) )
    float range[] = { 0, 256 };
    const float* histRange = { range };

    bool uniform = true;
    bool accumulate = false;

    /// Compute the histograms
    calcHist(&src, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

    // Draw the histograms for B, G and R
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);

    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

    /// Normalize the result to [ 0, histImage.rows ]
    normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    /// Draw for each channel
    for (int i = 1; i < histSize; i++)
    {
        line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(hist.at<float>(i))),
            cv::Scalar(255, 0, 0), 2, 8, 0);
    }
}

void plot(cv::Mat imagen, string nombre) {
    cv::Mat b_hist, g_hist, r_hist;
    /// Separate the image in 3 places ( B, G and R )
    vector<cv::Mat> bgr_planes;
    vector<cv::Mat> bgr_planesEqu;
    //es la funcion split la que separa la imagen en colores
    split(imagen, bgr_planes);

    /// Establish the number of bins
    int histSize = 256;

    /// Set the ranges ( for B,G,R) )
    float range[] = { 0, 256 };
    const float* histRange = { range };

    bool uniform = true;
    bool accumulate = false;

    /// Compute the histograms:
    //la funcion calcHist calcula el histograma autormaticamente
    cv::calcHist(&bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
    cv::calcHist(&bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
    cv::calcHist(&bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

    // Draw the histograms for B, G and R
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);

    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

    /// Normalize the result to [ 0, histImage.rows ]
    cv::normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    /// Draw for each channel
    for (int i = 1; i < histSize; i++)
    {
        line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
            cv::Scalar(255, 0, 0), 2, 8, 0);
        line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
            cv::Scalar(0, 255, 0), 2, 8, 0);
        line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
            cv::Scalar(0, 0, 255), 2, 8, 0);
    }

    cv::namedWindow("Histograma " + nombre, cv::WINDOW_AUTOSIZE);
    cv::imshow("Histograma " + nombre, histImage);
    cv::waitKey(0);
    cv::imshow("Imagen " + nombre, imagen);
    cv::waitKey(0);
}

void histograma(cv::Mat frame) {
    plot(frame, "Original");
    plotIntensity(frame, "before");

    cv::Mat hsv_final, contraste, ambasr, hsv_merge;

    // HSV equalization
    vector<cv::Mat> hsv_channels;
    cv::Mat HSV;
    cvtColor(frame, HSV, cv::COLOR_BGR2HSV);
    split(HSV, hsv_channels);
    equalizeHist(hsv_channels[2], hsv_channels[2]);     // Equalization (channel V)

    merge(hsv_channels, hsv_merge);
    cvtColor(hsv_merge, hsv_final, cv::COLOR_HSV2BGR);

    plot(hsv_final, "Ecualizada (canal v de HSV)");

    // RGB
    // Contrast and brightness
    double a = 1.4; // gain [1.0-3.0]
    double b = 20;  // offset [0-100]
    
    vector<cv::Mat> colors;
    split(frame, colors);
    colors[0] = colors[0] * a + b;
    colors[1] = colors[1] * a + b;
    colors[2] = colors[2] * a + b;

    merge(colors, contraste);
    plot(contraste, "Contraste y brillo en RGB");

    // HSV
    vector<cv::Mat> aux;
    split(HSV, aux);
    aux[2] = aux[2] * a + b;
    cv::Mat dst_test, test;
    merge(aux, dst_test);
    cvtColor(dst_test, test, cv::COLOR_HSV2BGR);

    merge(hsv_channels, hsv_merge);
    cvtColor(hsv_merge, hsv_final, cv::COLOR_HSV2BGR);
    
    plot(test, "Contraste y brillo en HSV");
    plotIntensity(hsv_final, "after");
}


cv::Mat alienEffect(cv::Mat frame, int color) {
    // Convert RGB to HSV
    cv::Mat frame_hsv;
    cv::cvtColor(frame, frame_hsv, cv::COLOR_BGR2HSV);

    switch (color)
    {
    case 0:     // Green
        frame_hsv = alienEffectAux(frame_hsv, 60);
        break;
    case 1:     // Blue
        frame_hsv = alienEffectAux(frame_hsv, 120);
        break;
    case 2:     // Red
        frame_hsv = alienEffectAux(frame_hsv, 0);
        break;
    default:
        cout << "Option not valid" << endl;
        break;
    }
    // Convert HSV to RGB
    cv::cvtColor(frame_hsv, frame, cv::COLOR_HSV2BGR);
    return frame;
}


cv::Mat alienEffectAux(cv::Mat frame_hsv, int value) {
    for (int i = 0; i < frame_hsv.rows; i++) {		
        uchar* data = frame_hsv.ptr<uchar>(i);
        for (int j = 0; j < frame_hsv.cols * frame_hsv.channels(); j = j + 3) {
            // Check the range of skin color to change value
            if (((data[j] >= 0 && data[j] <= 20) || (data[j] >= 170 && data[j] <= 180))) {
                data[j] = value;
            }
        }
    }
    return frame_hsv;
}


cv::Mat posterEffect(cv::Mat frame, int numColors) {
    cv::Mat imagenVector = cv::Mat(frame.cols * frame.rows, 3, CV_32F);
    // Format data to use k-means function
    for (int i = 0; i < frame.rows; i++) {
        for (int j = 0; j < frame.cols; j++) {
            imagenVector.at<float>(i + j * frame.rows, 0) = frame.at<cv::Vec3b>(i, j)[0];
            imagenVector.at<float>(i + j * frame.rows, 1) = frame.at<cv::Vec3b>(i, j)[1];
            imagenVector.at<float>(i + j * frame.rows, 2) = frame.at<cv::Vec3b>(i, j)[2];
        }
    }

    cv::Mat centroides; cv::Mat etiq;

    kmeans(imagenVector, numColors, etiq, cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 1, 1.0),
        1, cv::KMEANS_PP_CENTERS, centroides);

    // Create final image
    for (int i = 0; i < frame.rows; i++) {	
        for (int j = 0; j < frame.cols; j++) {
            int indice = etiq.at<int>(i + j * frame.rows, 0);
            frame.at<cv::Vec3b>(i, j)[0] = centroides.at<float>(indice, 0);
            frame.at<cv::Vec3b>(i, j)[1] = centroides.at<float>(indice, 1);
            frame.at<cv::Vec3b>(i, j)[2] = centroides.at<float>(indice, 2);
        }
    }
    return frame;
}


cv::Mat distortion(cv::Mat frame, double coeficient) {
    double cX = frame.rows / 2;
    double cY = frame.cols / 2;
    double k = coeficient / 1000000.0;

    cv::Mat coordX, coordY;
    coordX.create(frame.size(), CV_32FC1);
    coordY.create(frame.size(), CV_32FC1);

    for (double x=0; x<coordX.rows; x++) {
        for (double y = 0; y < coordX.cols; y++) {
            double r2 = (x - cX) * (x - cX) + (y - cY) * (y - cY);
            double x_corr = ((y - cY) / (1 + k*r2) + cY);
            double y_corr = ((x - cX) / (1 + k*r2) + cX);
            coordX.at<float>(x, y) = x_corr;
            coordY.at<float>(x, y) = y_corr;
        }
    }

    cv::remap(frame, frame, coordX, coordY, cv::INTER_LINEAR);

    return frame;
}

