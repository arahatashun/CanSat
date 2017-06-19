#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "camera.hpp"

#pragma comment(lib,"opencv_world320.lib")

int main(void)
{
	cv::Mat red = Mred(takePhoto());
	countArea(red);
	getCenter(red);
}
