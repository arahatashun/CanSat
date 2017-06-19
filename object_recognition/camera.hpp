#ifndef CAMERA_H
#define CAMERA_H
#include <opencv2/opencv.hpp>

char* takePhoto(void);
cv::Mat Mred(char* full_path);
double countArea(cv::Mat src);
double getCenter(cv::Mat src);

#endif
