#include <stdlib.h>
#include <stdlib.h>
#include <time.h>
#include "camera.hpp" 

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
