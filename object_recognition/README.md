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
