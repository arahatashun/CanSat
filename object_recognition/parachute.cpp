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
	countRed(takePhoto());
}
