# cansat

## ログの取り方

```shell
$sudo stdbuf -o0 -e0 ./a.out | tee log/`date +%Y%m%d_%H-%M-%S`.txt
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


## NOTE

I2Cが絡むコードを動かすときは全てsudoをつけること
