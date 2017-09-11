#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>

int main()
{
cout<<"enter path";
string parh;
cin>>path;
cv::Mat input = imread(path);
cv::Mat hsv_filtered15;//画像の初期化
cv::Mat hsv_filtered180;//画像の初期化
cv::cvtColor(input,hsv,CV_BGR2HSV);//入力画像(src)をhsv色空間(dst)に変換
//inRange(入力画像,下界画像,上界画像,出力画像)
//「HSV」は、色を色相(Hue)・彩度(Saturation)・明度(Value)
cv::inRange(hsv,cv::Scalar(0,120,97),cv::Scalar(13,255,MAX_VALUE),hsv_filtered15);
cv::inRange(hsv,cv::Scalar(175,120,97),cv::Scalar(180,255,MAX_VALUE),hsv_filtered180);
cv::add(hsv_filtered15,hsv_filtered180,output;
imwrite(path+"BINARY"+"jpg",output);
}
