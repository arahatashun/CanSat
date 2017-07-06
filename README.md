# cansat

## ログの取り方

ログの保存場所は~/Documentsに設定します.


```shell
$sudo stdbuf -o0 -e0 ./a.out | tee ~/Documents/`date +%Y%m%d_%H-%M-%S`.txt
```

## めんどくさいのでshell scriptを使ったやり方

```shell
$sudo sh log.sh
```
とすると
```shell
Enter program name (excluding .out)
```
と出るので
プログラム名を入れます

## I2C系素子の接触確認方法

```shell
$sudo i2cdetect -y 1
```

でI2C系素子の接触確認ができる。

## NOTE

I2Cが絡むコードを動かすときは全てsudoをつけること

## コンパスのoffset値
FM2台目搭載コンパス

`COMPASS_X_OFFSET = 35.5`

`COMPASS_Y_OFFSET = -545.0`

FM1台目搭載コンパス

`COMPASS_X_OFFSET = 97.0`

`COMPASS_Y_OFFSET = -258.0`

## PIDパラメーターの調整について

Dはとりあえず無視.Iもとりあえず無視してPをまず決めます.
Pは首振りしない限界の値を二分探索で求めます.そのあとIを足していって首振りしない限界まで近づける感じです.
