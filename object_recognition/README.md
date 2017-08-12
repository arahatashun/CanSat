## ライブラリーのインストール
opencvをインストールする
## 写真の保存される場所
```shell
/home/pi/Pictures
```
## scpコマンドでの写真の転送
今いるディレクトリに転送される
```shell
scp pi@raspberrypi.local:/home/pi/Pictures/*.jpg .
```
## In order to enable the camera for opencv automatically
```shell
sudo modprobe bcm2835-v4l2
```
## 起動時にカーネルモジュールの組み込みを行うために
/etc/modulesにbcm2835-v4l2を追加する

## 画像の転送
```shell
scp pi@raspberrypi.local:/home/pi/Pictures/*.jpg ~/Desktop
```
