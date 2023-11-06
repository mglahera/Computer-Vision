@echo off

g++ -std=c++11 -o L4 L4.cpp -I C:\opencv\build\include -L C:\opencv\build\lib -l libopencv_calib3d450 -l libopencv_core450 -l libopencv_dnn450 -l libopencv_features2d450 -l libopencv_flann450 -l libopencv_gapi450 -l libopencv_highgui450 -l libopencv_imgcodecs450 -l libopencv_imgproc450 -l libopencv_ml450 -l libopencv_objdetect450 -l libopencv_photo450 -l libopencv_stitching450 -l libopencv_video450 -l libopencv_videoio450 -l libopencv_xfeatures2d450

if errorlevel 1 (echo error compiling) else (L4 "disk")