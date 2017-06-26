#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "camera.hpp"

#pragma comment(lib,"opencv_world320.lib")

static const int N = 256;//文字列の長さ
static const int AOV = 62.2;//ANGLE OF VIEW
//明度について
static const int MAX_VALUE = 255;//明るさ最大
static const int NO_VALUE = 0;//明るさ最小


//時間を元にStringを作る
char* makeTimeString(void)
{
	static char timeString[N];
	time_t timer;//時刻を受け取る変数
	struct tm *timeptr;//日時を集めた構造体ポインタ
	time(&timer);//現在時刻の取得
	timeptr = localtime(&timer);//ポインタ
	strftime(timeString, N, "%Y%m%d-%H%M%S", timeptr);//日時を文字列に変換してｓに代入
	return timeString;
}

char* makeBinaryString(char *timeString)
{
	static char binaryString[N];
	sprintf(binaryString, "%s%s",timeString,"Binary");
}

//full_pathを作る
char* makePath(char* name)
{
	static char full_path[N];//NOTE 自動変数をreturn するために使った. smartなやり方か?
	char directry_path[] = "/home/pi/Pictures/";//pathの先頭
	char file_extention[] = ".jpg";//拡張子
	sprintf(full_path, "%s%s%s",directry_path,name, file_extention);
	return full_path;
}
//写真をとる
int takePhoto(char* name)
{
	char full_command[N];
	char front_command[] = "raspistill -o ";//command
	sprintf(full_command, "%s%s", front_command, makePath(name));//コマンドの文字列をつなげる。
	system(full_command);//raspistillで静止画を撮って日時を含むファイル名で保存。
	printf("%s\n",full_command);
	//NOTE system関数以外を使うべきか?
	return 0;
}

//二値化画像を作成
cv::Mat binarize(cv::Mat src)
{
	cv::Mat hsv;
	cv::Mat hsv_filtered15 ;//画像の初期化
	cv::Mat hsv_filtered180;//画像の初期化
	cv::cvtColor(src, hsv, CV_BGR2HSV);//入力画像(src)をhsv色空間(dst)に変換
	//inRange(入力画像,下界画像,上界画像,出力画像)
	//「HSV」は、色を色相(Hue)・彩度(Saturation)・明度(Value)
	cv::inRange(hsv, cv::Scalar(0, 45, 70), cv::Scalar(12, 255, MAX_VALUE), hsv_filtered15);
	cv::inRange(hsv, cv::Scalar(160, 45, 70), cv::Scalar(180, 255, MAX_VALUE), hsv_filtered180);
	cv::add(hsv_filtered15,hsv_filtered180,hsv);
	return hsv;
}
//ノイズ除去
cv::Mat rmNoize(cv::Mat src)
{
	cv::erode(src,src,cv::Mat(),cv::Point(-1, -1),10);//縮小処理
	cv::dilate(src,src,cv::Mat(),cv::Point(-1, -1),25);//膨張処理
	cv::erode(src,src,cv::Mat(),cv::Point(-1, -1),15);//縮小処理
	return src;
}

int saveBinary(cv::Mat src,char* path)
{
	cv::Mat binary_img;
	cv::resize(src,binary_img,cv::Size(),0.25,0.25);
	return 0;
}

//写真を撮り画像処理する
cv::Mat Mred(void)
{
	char* stime = makeTimeString();
	char* sbtime = makeBinaryString(btime);
	char* path = makePath(stime);
	char* bpath = makePath(sbtime);
	takePhoto(path);
 	cv::Mat src = cv::imread(path);//画像の読み込み
	cv::Mat hsv;
	hsv = rmNoize(binarize(src));
	saveBinary(hsv,bpath);
	return hsv;
}


//二値化した画像から1の面積を抽出
double countArea(cv::Mat src)
{

	double Area = src.rows*src.cols;//全ピクセル数
	double redCount = 0; //赤色を認識したピクセルの数
	redCount = cv::countNonZero(src);//赤色部分の面積を計算
	double percentage = 0; //割合
	percentage = (redCount / Area)*100;//割合を計算
	printf("面積のPercentageは%f\n", percentage);
	return percentage;
}

//二値化画像のcenterを角度で返す
double getCenter(cv::Mat src)
{
	cv::Moments mu = cv::moments(src, false);//重心の計算結果をmuに代入
	double mc = mu.m10 / mu.m00;//重心のx座標
	double center = (mc - src.cols / 2) * AOV / src.cols;//正規化
	printf("重心の位置は%f\n",center);
	return center;
}
