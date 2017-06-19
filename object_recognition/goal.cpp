#include <stdlio.h>
#include <time.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "camera.hpp"

#pragma comment(lib,"opencv_world320.lib")

int main(void) {
	cv::Mat src,hsv,hsv_filtered15,hsv_filtered180;
	dst_filtered15 = cv::Scalar(0, 0, 0);//画像の初期化
	dst_filtered180 = cv::Scalar(0, 0, 0);//画像の初期化
	src =cv::imread(takePhoto());//画像の読み込み
	double Area = src.rows*src.cols;//全ピクセル数
	cv::cvtColor(src, hsv, CV_BGR2HSV);//入力画像(src)をhsv色空間(dst)に変換
	//inRange(入力画像,下界画像,上界画像,出力画像)
	//「HSV」は、色を色相(Hue)・彩度(Saturation)・明度(Value)
	cv::inRange(hsv, cv::Scalar(0, 150, 0), cv::Scalar(15, 255, 255), hsv_filtered15);
	cv::inRange(hsv, cv::Scalar(160, 150, 0), cv::Scalar(180, 255, 255), hsv_filtered180);
	cv::add(hsv_filtered15,hsv_filtered180,hsv);
	double red_count = 0; //赤色を認識したピクセルの数
	red_count = cv::countNonZero(hsv);//赤色部分の面積を計算
	double percentage = 0; //割合
	percentage = (redcount / Area)*100;//割合を計算
	printf("赤色の面積の割合は%f\n", percentage);
	cv::Moments mu = cv::moments(hsv, false);//重心の計算結果をmuに代入
	double mc = mu.m10 / mu.m00;//重心のx座標
	double center = (mc - dst.cols / 2) * 200 / dst.cols;//正規化
	printf("重心の位置は%f\n",center);
}
