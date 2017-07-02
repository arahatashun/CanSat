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
`$sudo i2cdetect -y 1`

でI2C系素子の接触確認ができる。

## NOTE

I2Cが絡むコードを動かすときは全てsudoをつけること
