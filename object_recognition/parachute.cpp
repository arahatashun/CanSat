#include<stdlib.h>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<time.h>
#pragma comment(lib,"opencv_world320.lib")

static const int N = 256;//文字列の長さ
int main(void) {
	char full_command[N];
	char front_command[] = "sudo raspistill -o ";//command
	char full_path[N];
	char directry_path[] = "/home/pi/object";//pathの先頭
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

	cv::Mat src,dst,dst_filtered;
	dst_filtered = cv::Scalar(0, 0, 0);//画像の初期化
	double count = 0; //赤色を認識したピクセルの数
	double percentage = 0; //割合
	src =cv::imread(full_path);
	cv::cvtColor(src, dst, CV_BGR2HSV);//入力画像(src)をhsv色空間(dst)に変換
	//inRange(入力画像,下界画像,上界画像,出力画像)
	//「HSV」は、色を色相(Hue)・彩度(Saturation)・明度(Value)
	cv::inRange(dst, cv::Scalar(160, 150, 0), cv::Scalar(190, 255, 255), dst_filtered);
	count = cv::countNonZero(dst_filtered);//赤色部分の面積を計算
	double Area = dst.rows*dst.cols;//全ピクセル数の計算
	percentage = count / Area;//割合を計算
	printf("赤色の面積の割合は%f\n", percentage);
}
