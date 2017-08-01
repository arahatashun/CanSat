#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#pragma comment(lib,"opencv_world320.lib")

class Camera
{
private:
  void operator=(const Camera&){};
  Camera(const Camera&){};
  int makeTimePath();
  cv::Mat rmNoise(cv::Mat src);
  cv::VideoCapture capture;
  std::string timePath;
  cv::Mat input;//入力画像
  cv::Mat output;//出力画像
public:
  Camera();
  ~Camera();
  int takePhoto();
  int binarize();
  double countArea();
  double getCenter();
};

#endif
