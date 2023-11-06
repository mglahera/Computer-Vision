#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;

int diskMode(vector<cv::Mat>, vector<cv::Mat>);
void liveMode(vector<cv::Mat>, vector<cv::Mat>);
int panorama(vector<cv::Mat>, vector<cv::Mat>);
void calculateMinMax(vector<vector<cv::Point2f>>, vector<cv::Point2f>, vector<float>*);


int main(int argc, char* argv[]) 
{
    if(argc != 2) 
    {
        cout << "Usage: L4 <'disk'|'live'>" << endl;
        return -1;
    }

    vector<cv::Mat> imgsBW;         // Grayscale images
    vector<cv::Mat> imgsColor;      // Color images

    string mode = argv[1];

    if(mode == "disk") diskMode(imgsBW, imgsColor);
    else if(mode == "live") liveMode(imgsBW, imgsColor);
    else 
    {
        cout << "Usage: L4 <'disk'|'live'>" << endl;
        return -1;
    }

    return 0;
}


int diskMode(vector<cv::Mat> imgsBW, vector<cv::Mat> imgsColor)
{
    cout << "Write the path of the image directory: ";
    string path;
    cin >> path;

    vector<string> imgsPath;            // Images paths
    cv::glob(path, imgsPath, false);

    // Load images
    for(int i=0; i<imgsPath.size(); i++)
    {
        string path = imgsPath[i];
        replace(path.begin(), path.end(), '\\', '/');

        cout << "Reading:  " << path << endl;
        cv::Mat aux = cv::imread(path, cv::IMREAD_COLOR);   // Color image

        cv::resize(aux, aux, cv::Size(aux.cols * 0.75, aux.rows * 0.75));       // Resize

        cv::Mat aux2;
        cv::cvtColor(aux, aux2, cv::COLOR_BGR2GRAY);        // Black and white

        imgsColor.push_back(aux);
        imgsBW.push_back(aux2);
    }

    panorama(imgsBW, imgsColor);
    
    return 0;
}


void liveMode(vector<cv::Mat> imgsBW, vector<cv::Mat> imgsColor)
{
    int imagesTaken = 0;
    while(imagesTaken < 3)
    {
        cv::Mat img;
        cout << "Waiting for image, press SPACE" << endl;
        cv::VideoCapture cap(0); // open the default camera
        cap >> img;
        imshow("Image captured", img);
        int keyP = cv::waitKey(0);
        imgsColor.push_back(img);
        cv::Mat imgG;
        cvtColor(img, imgG, cv::COLOR_BGR2GRAY);
        imgsBW.push_back(imgG);
        imagesTaken++;
    }

    panorama(imgsBW, imgsColor);
}


int panorama(vector<cv::Mat> imgsBW, vector<cv::Mat> imgsColor) 
{
    // Select type of scene
    int typeScene = 0;
    cout << "\nSelect type of scene:" << endl;
    cout << " 2D----> 1" << endl;
    cout << " 3D----> 2" << endl;
    cout << ">";
    cin >> typeScene;

    cv::Mat coreCL;
    if(typeScene == 1)          // 2D
    {
        // Take first image as core
        coreCL = imgsColor.at(0);
        imgsColor.erase(imgsColor.begin());
        imgsBW.erase(imgsBW.begin());
    }
    else if(typeScene == 2)     // 3D
    {
        // Take the middle image as core
        coreCL = imgsColor.at(imgsBW.size()/2 - 1);
        imgsColor.erase(imgsColor.begin() + imgsBW.size()/2 - 1);
        imgsBW.erase(imgsBW.begin() + imgsBW.size()/2 - 1);
    }
    else
    {
        cout << "ERROR: not scene type selected" << endl;
        return -1;
    }

    // Select the feature extraction method
    int detectorExtractor = 0;
    cout << "\nSelect the feature extraction method:" << endl;
    cout << " ORB-----> 1" << endl;
    cout << " SIFT----> 2" << endl;
    cout << " SURF----> 3" << endl;
    cout << " AKAZE---> 4" << endl;
    cout << ">";
    cin >> detectorExtractor;

    // Select the matching method
    int matcherType = 0;
    cout << "\nSelect the matching method:" << endl;
    cout << " Brute-----> 1" << endl;
    cout << " Flann-----> 2" << endl;
    cout << ">";
    cin >> matcherType;

    auto t1 = chrono::high_resolution_clock::now();
    cout << "Images: " << imgsBW.size() << endl;
    for(int i=0; i<imgsBW.size(); i++) 
    {
        cv::Mat core;
        cv::Mat add = imgsBW.at(i);
        cv::Mat addC = imgsColor.at(i);
        cvtColor(coreCL, core, cv::COLOR_BGR2GRAY);

        vector<cv::KeyPoint> keypointsCore, keypointsAdd;
        cv::Mat descriptorsCore, descriptorsAdd;

        if(detectorExtractor == 1)          // ORB
        {
            //cv::Ptr<cv::ORB> detector = cv::ORB::create(2000);
            cv::Ptr<cv::ORB> detector = cv::ORB::create();
            detector->detect(core, keypointsCore);
            detector->detect(add, keypointsAdd);
            cv::Ptr<cv::ORB> extractor = cv::ORB::create();
            extractor->compute(core, keypointsCore, descriptorsCore);
            extractor->compute(add, keypointsAdd, descriptorsAdd);
        }
        else if(detectorExtractor == 2)     // SURF
        {
            //cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(200);
            cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create();
            detector->detect(core, keypointsCore);
            detector->detect(add, keypointsAdd);
            cv::Ptr<cv::xfeatures2d::SURF> extractor = cv::xfeatures2d::SURF::create();
            extractor->compute(core, keypointsCore, descriptorsCore);
            extractor->compute(add, keypointsAdd, descriptorsAdd);
        }
        else if(detectorExtractor == 3)     // SIFT
        {
            //cv::Ptr<cv::SIFT> detector = cv::SIFT::create(200);
            cv::Ptr<cv::SIFT> detector = cv::SIFT::create();
            detector->detect(core, keypointsCore);
            detector->detect(add, keypointsAdd);
            cv::Ptr<cv::SIFT> extractor = cv::SIFT::create();
            extractor->compute(core, keypointsCore, descriptorsCore);
            extractor->compute(add, keypointsAdd, descriptorsAdd);
        }
        else if(detectorExtractor == 4)     // AKAZE
        {
            cv::Ptr<cv::AKAZE> akaze = cv::AKAZE::create();
            akaze->detectAndCompute(core, cv::noArray(), keypointsCore, descriptorsCore);
            akaze->detectAndCompute(add, cv::noArray(), keypointsAdd, descriptorsAdd);
        }
        else 
        {
            cout << "ERROR: no feature extraction method selected" << endl;
            return -1;
        }
                
        // Finds matches between both images (core and other)
        vector<vector<cv::DMatch>> matches;
        if (matcherType == 1)                   // Brute
        {
            if (detectorExtractor == 1)
            {
                cv::BFMatcher matcher(cv::NORM_HAMMING);
                matcher.knnMatch(descriptorsAdd, descriptorsCore, matches, 2);
            }
            else
            {
                cv::BFMatcher matcher(cv::NORM_L2);
                matcher.knnMatch(descriptorsAdd, descriptorsCore, matches, 2);
            }
        }
        else if (matcherType == 2)              // Flann
        {
            if(descriptorsCore.type()!=CV_32F) descriptorsCore.convertTo(descriptorsCore, CV_32F);
            if(descriptorsAdd.type()!=CV_32F) descriptorsAdd.convertTo(descriptorsAdd, CV_32F);
            cv::FlannBasedMatcher matcher;
            matcher.knnMatch(descriptorsAdd, descriptorsCore, matches, 2);
        }

        // Only good matches are preserved (2nd neighbor ratio)
        vector<cv::DMatch> good_matches;
        for (int i = 0; i < (int)matches.size(); i++)
        {
            float d1 = matches.at(i).at(0).distance;
            float d2 = matches.at(i).at(1).distance;
            if (d1 < d2 * 0.7) good_matches.push_back(matches.at(i).at(0));
        }

        // Draw matches
        cv::Mat img_matches;
        drawMatches(add, keypointsAdd, core, keypointsCore, good_matches, img_matches);

        // Localize the object 
        vector<cv::Point2f> picCoreAdd;
        vector<cv::Point2f> picAddCore;
        for (int i = 0; i < (int)good_matches.size(); i++)
        {
            // Get the keypoints from the good matches 
            picCoreAdd.push_back(keypointsAdd[good_matches[i].queryIdx].pt);
            picAddCore.push_back(keypointsCore[good_matches[i].trainIdx].pt);
        }

        if (good_matches.size() >= 4)
        {
            cv::Mat H = findHomography(picCoreAdd, picAddCore, cv::FM_RANSAC);          // RANSAC
           
            // Get the corners from the image_1 ( the object to be "detected" ) 
            vector<cv::Point2f> obj_c12(4);
            vector<cv::Point2f> core_c(4);
            obj_c12[0] = cv::Point(0, 0);
            obj_c12[1] = cv::Point(add.cols, 0);
            obj_c12[2] = cv::Point(add.cols, add.rows);
            obj_c12[3] = cv::Point(0, add.rows);

            core_c[0] = cv::Point(0, 0);
            core_c[1] = cv::Point(core.cols, 0);
            core_c[2] = cv::Point(core.cols, core.rows);
            core_c[3] = cv::Point(0, core.rows);

            vector<cv::Point2f> scene_c12(4);
            perspectiveTransform(obj_c12, scene_c12, H);
            
            // Draw lines between the corners
            line(img_matches, scene_c12[0] + cv::Point2f(add.cols, 0), 
                scene_c12[1] + cv::Point2f(add.cols, 0),
                cv::Scalar(0, 255, 0), 4);
            line(img_matches, scene_c12[1] + cv::Point2f(add.cols, 0),
                scene_c12[2] + cv::Point2f(add.cols, 0),
                cv::Scalar(0, 255, 0), 4);
            line(img_matches, scene_c12[2] + cv::Point2f(add.cols, 0),
                scene_c12[3] + cv::Point2f(add.cols, 0),
                cv::Scalar(0, 255, 0), 4);
            line(img_matches, scene_c12[3] + cv::Point2f(add.cols, 0),
                scene_c12[0] + cv::Point2f(add.cols, 0),
                cv::Scalar(0, 255, 0), 4);
            
            vector<vector<cv::Point2f>> scenes; 
            scenes.push_back(scene_c12);
            vector<float> minMax(4);
            calculateMinMax(scenes, core_c, &minMax);
            int width = minMax[1] - minMax[0];
            int height = minMax[3] - minMax[2];

            cv::Mat T = cv::Mat::eye(3, 3, CV_64FC1);
            if (minMax[0] < 0) T.at<double>(0, 2) = -minMax[0];
            if (minMax[2] < 0) T.at<double>(1, 2) = -minMax[2];

            // Use the Homography Matrix to warp the images 
            cv::Mat result;
            result = cv::Mat(cv::Size(width, height), CV_32F);
            warpPerspective(coreCL, result, T, result.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);
            warpPerspective(addC, result, T * H, result.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);

            cv::Mat res = result;
            resize(res, res, cv::Size(res.cols * 0.4, res.rows * 0.4));
            imshow("Result", res);
            coreCL = result;

            // Show detected matches 
            cv::Mat matchesShow;
            resize(img_matches, matchesShow, cv::Size(img_matches.cols * 0.5, img_matches.rows * 0.5));
            imshow("Matches", matchesShow);

            cv::waitKey(0);
        }
        else
        {
            cout << "Not enought good matches" << endl;
        }
    }
    auto t2 = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms_double = t2 - t1;
    cout << ms_double.count() << "ms" << endl;

    return 0;
}


void calculateMinMax(vector<vector<cv::Point2f>> objs, vector<cv::Point2f> core, vector<float> *sal)
{
    float minX = numeric_limits<int>::max();
    float minY = numeric_limits<int>::max();
    float maxX = -numeric_limits<int>::max();
    float maxY = -numeric_limits<int>::max();
    for (int j = 0; j < objs.size(); ++j)
    {
        vector<cv::Point2f> obj = objs.at(j);
        for (int i = 0; i < 4; ++i)
        {
            float objX = obj[i].x;
            float objY = obj[i].y;
            float coreX = core[i].x;
            float coreY = core[i].y;
            (objX < minX) ? (minX = objX) : (minX);
            (objX > maxX) ? (maxX = objX) : (maxX);
            (objY < minY) ? (minY = objY) : (minY);
            (objY > maxY) ? (maxY = objY) : (maxY);
            (coreX < minX) ? (minX = coreX) : (minX);
            (coreX > maxX) ? (maxX = coreX) : (maxX);
            (coreY < minY) ? (minY = coreY) : (minY);
            (coreY > maxY) ? (maxY = coreY) : (maxY);
        }
    }
    sal->at(0) = minX;
    sal->at(1) = maxX;
    sal->at(2) = minY;
    sal->at(3) = maxY;
}