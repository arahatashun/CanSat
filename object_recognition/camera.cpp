#include <ctime>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "camera.hpp"
#include <vector>

static const int WIDTH = 640;
static const int HEIGHT = 480;
static const int FPS = 6;
static const int DEVICE = 0;
static const std::string DIRECTORY_PATH = "/home/pi/Pictures/";//pathの先頭
static const std::string FILE_EXTENTION = ".jpg";//拡張子

static const int AOV = 62.2;//ANGLE OF VIEW
//明度について
static const int MAX_VALUE = 255;//明るさ最大
static const int NO_VALUE = 0;//明るさ最小
static const double MIN_AREA = 1;//抽出する面積の最小値

Camera::Camera()
{
	if(!capture.open(DEVICE))
	{
		std::cout<<"capture is not opened 1"<<std::endl;
	}
	if (!capture.isOpened())
	{
		std::cout<<"capture is not opened 2"<<std::endl;
	}
	capture.set(CV_CAP_PROP_FRAME_WIDTH,WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,HEIGHT);
	capture.set(CV_CAP_PROP_FPS,FPS);
	//以下 対応してない
	//capture.set(CV_CAP_PROP_BUFFERSIZE, 1);
}

Camera::~Camera()
{
	capture.release();
}

int Camera::takePhoto()
{
	makeTimePath();
	cv::Mat frame;
	for (int i = 0; i < 5; i++)
	{
			capture.grab();
	}
	do
	{
		capture>>frame;
	} while(frame.empty());
	input = frame;
	imwrite(timePath+FILE_EXTENTION,input);
	return 0;
}

//時間を元にtimePathを作る
int Camera::makeTimePath(void)
{
	time_t timer;//時刻を受け取る変数
	struct tm *timeptr;//日時を集めた構造体ポインタ
	char buffer[80];
	time(&timer);//現在時刻の取得
	timeptr = localtime(&timer);//ポインタ
	strftime(buffer, sizeof(buffer), "%Y%m%d-%H%M%S", timeptr);//日時を文字列に変換してｓに代入
	std::string str(buffer);
	timePath = DIRECTORY_PATH+str;
  return 0;
}

//ノイズ除去,引数aは抽出する輪郭の面積の最小値
cv::Mat Camera::rmNoise(cv::Mat src)
{
	std::vector<std::vector<cv::Point> > contours;//輪郭座標の二次元配列
	cv::findContours(src,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);//CV_RETR_EXTERNAL:最も外側の輪郭を検出、CV_CHAIN_APPROX_NONE:近似手法
	std::vector<std::vector<cv::Point> > contours_subset;
	for (int i = 0;i<contours.size();i++)
	{
		double area = cv::contourArea(contours.at(i));//各輪郭の面積
		if (area>MIN_AREA)
		{
			contours_subset.push_back(contours.at(i));//contours_subsetに一定面積以上の輪郭を追加
		}
	}
	cv::Mat mask = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
	cv::drawContours(mask, contours_subset, -1, cv::Scalar(255), -1);//maskに一定面積以上の輪郭をdraw
	return mask;
}

int Camera::binarize()
{
	cv::Mat hsv;
	cv::Mat hsv_filtered15;//画像の初期化
	cv::Mat hsv_filtered180;//画像の初期化
	cv::cvtColor(input,hsv,CV_BGR2HSV);//入力画像(src)をhsv色空間(dst)に変換
	//inRange(入力画像,下界画像,上界画像,出力画像)
	//「HSV」は、色を色相(Hue)・彩度(Saturation)・明度(Value)
	cv::inRange(hsv,cv::Scalar(0,70,60),cv::Scalar(2,255,MAX_VALUE),hsv_filtered15);
	cv::inRange(hsv,cv::Scalar(160,70,60),cv::Scalar(180,255,MAX_VALUE),hsv_filtered180);
	cv::add(hsv_filtered15,hsv_filtered180,hsv);
	output = rmNoise(hsv);
	imwrite(timePath+"BINARY"+FILE_EXTENTION,output);
	return 0;
}


//二値化した画像から1の面積を抽出
double Camera::countArea()
{

	double Area = output.rows*output.cols;//全ピクセル数
	double redCount = 0; //赤色を認識したピクセルの数
	redCount = cv::countNonZero(output);//赤色部分の面積を計算
	double percentage = 0; //割合
	percentage = (redCount / Area)*100;//割合を計算
	std::cout<<"面積のPercentageは"<<percentage<<std::endl;
	return percentage;
}

//二値化画像のcenterを角度で返す
double Camera::getCenter()
{
	cv::Moments mu = cv::moments(output, false);//重心の計算結果をmuに代入
	double mc = mu.m10 / mu.m00;//重心のx座標
	double center = (mc - output.cols / 2) * AOV / output.cols;//正規化
	std::cout<<"重心の位置は"<<center<<std::endl;
	return center;
}
