#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdlio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "camera.hpp"

#pragma comment(lib,"opencv_world320.lib")

static const int N = 256;//文字列の長さ

//写真をとってそのpathを返す
char* takePhoto(void)
{
	char full_command[N];
	char front_command[] = "raspistill -o ";//command
	static char full_path[N];//NOTE 自動変数をreturn するために使った. smartなやり方か?
	char directry_path[] = "/home/pi/Pictures/";//pathの先頭
	char name_path[N];//時間を文字列に変換するときに代入する変数
	char file_extention[] = ".jpg";//拡張子
	time_t timer;//時刻を受け取る変数
	struct tm *timeptr;//日時を集めた構造体ポインタ
	time(&timer);//現在時刻の取得
	timeptr = localtime(&timer);//ポインタ
	strftime(name_path, N, "%Y%m%d-%H%M%S", timeptr);//日時を文字列に変換してｓに代入
	sprintf(full_path, "%s%s%s",directry_path, name_path, file_extention);
	sprintf(full_command, "%s%s", front_command, full_path);//コマンドの文字列をつなげる。
	system(full_command);//raspistillで静止画を撮って日時を含むファイル名で保存。
	printf("%s\n",full_command);
	//NOTE system関数以外を使うべきか?
	return full_path;
}

//入力画像を赤色に二値化
cv::Mat Mred(char* full_path)
{
	cv::Mat src,hsv,hsv_filtered15,hsv_filtered180;
	dst_filtered15 = cv::Scalar(0, 0, 0);//画像の初期化
	dst_filtered180 = cv::Scalar(0, 0, 0);//画像の初期化
	src =cv::imread(full_path);//画像の読み込み
	cv::cvtColor(src, hsv, CV_BGR2HSV);//入力画像(src)をhsv色空間(dst)に変換
	//inRange(入力画像,下界画像,上界画像,出力画像)
	//「HSV」は、色を色相(Hue)・彩度(Saturation)・明度(Value)
	cv::inRange(hsv, cv::Scalar(0, 150, 0), cv::Scalar(15, 255, 255), hsv_filtered15);
	cv::inRange(hsv, cv::Scalar(160, 150, 0), cv::Scalar(180, 255, 255), hsv_filtered180);
	cv::add(hsv_filtered15,hsv_filtered180,hsv);
	return hsv;
}

//二値化した画像から1の面積を抽出
double countArea(cv::Mat src)
{

	double Area = src.rows*src.cols;//全ピクセル数
	double red_count = 0; //赤色を認識したピクセルの数
	red_count = cv::countNonZero(src);//赤色部分の面積を計算
	double percentage = 0; //割合
	percentage = (redcount / Area)*100;//割合を計算
	printf("面積の割合は%f\n", percentage);
	return precentage;
}

//二値化画像のcenterを-100から100で返す
double getCenter(cv::Mat src)
{
	cv::Moments mu = cv::moments(src, false);//重心の計算結果をmuに代入
	double mc = mu.m10 / mu.m00;//重心のx座標
	double center = (mc - src.cols / 2) * 200 / src.cols;//正規化
	printf("重心の位置は%f\n",center);
	return center;
}
