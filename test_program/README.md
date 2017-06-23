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
