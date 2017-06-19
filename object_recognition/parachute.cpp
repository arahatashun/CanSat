#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "camera.hpp"

#pragma comment(lib,"opencv_world320.lib")

static const int N = 256;//文字列の長さ
//NOTE opencvの場合 hueは0～180に圧縮
static const int HUE_MIN = 160;
static const int HUE_MAX = 190;



int main(void) {
	cv::Mat src,hsv,hsv_filtered;
	hsv_filtered = cv::Scalar(0, 0, 0);//画像の初期化
	double count = 0; //赤色を認識したピクセルの数
	double percentage = 0; //割合
	src =cv::imread(takePhoto());
	cv::cvtColor(src, hsv, CV_BGR2HSV);//入力画像(src)をhsv色空間(hsv)に変換
	//inRange(入力画像,下界画像,上界画像,出力画像)
	//「HSV」は、色を色相(Hue)・彩度(Saturation)・明度(Value)
	cv::inRange(hsv, cv::Scalar(HUE_MIN, 150, 0), cv::Scalar(HUE_MAX, 255, 255), hsv_filtered);
	count = cv::countNonZero(hsv_filtered);//赤色部分の面積を計算
	double Area = hsv_filtered.rows*hsv_filtered.cols;//全ピクセル数の計算
	percentage = (count / Area)*100;//百分率で計算
	printf("赤色の面積の割合は%f\n", percentage);
}
