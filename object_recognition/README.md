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
## In order to enable the camera for openv automatically
```shell
sudo modprobe bcm2835-v4l2
```
