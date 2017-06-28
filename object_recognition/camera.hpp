#ifndef CAMERA_H
#define CAMERA_H
#include <opencv2/opencv.hpp>

cv::Mat Mred(void);
double countArea(cv::Mat src);
double getCenter(cv::Mat src);
int takePhoto;
#endif
