# cansat

## ログの取り方

```shell
sudo stdbuf -o0 -e0 ./a.out | tee log/`date +%Y%m%d`.txt
```

## NOTE

I2Cが絡むコードを動かすときは全てsudoをつけること
