# テスト関数フォルダ

テスト関数のコンパイル簡易化のために作りました。
基本的には

```shell
$ make センサ名(e.g. make xbee)
$ sudo ./センサ名.out(e.g. sudo ./xbee.out)
```

とすることで使用できます。

## テスト関数の名前一覧

|対象|コンパイル|実行|
|:--|:--|:--|
|モーター|make motor|sudo ./motor.out|
|テグス|make gut|sudo ./gut.out|
|ルクスセンサ|make lux|sudo ./lux.out|
|ジャイロセンサ|make gyro|sudo ./gyro.out|
|XBEE|make xbee|sudo ./xbee.out|
|GPS|make gps|sudo ./gps.out|
|轍回避|make escape|sudo ./escape.out|
## ssh接続してからの手順

```shell
$ ssh pi@raspberrypi.local
$ cd cansat
$ cd test_program
```

## 注意

makeを実行して
```shell
make: `センサー名' is up to date.
```
となった時はすでに実行ファイルがあると思うのでsudo ./センサー名.outとしてください。
