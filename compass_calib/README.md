# 地磁気キャリブレーション用フォルダ

このフォルダにあるのは

compass_scatter.c

(地磁気のx,yのraw_dataをcompasslog.txtというテキストファイルに書き込むプログラム)

compass_test.c(今は使ってない)

(マシンが自分でその場で回転しながらcalibrationを行い、キャリブレーションが終わったらそのあとcompass_angleを吐き続けるプログラム)

calib.py

(compasslog.txtをpythonでparseし、楕円プロットやoffset値などを計算するプログラム)

## 関数の名前一覧

|対象|コンパイル|実行|
|:--|:--|:--|
|compass_scatter.c|make scatter|sudo ./scatter.out|
|compass_test.c|make test|sudo ./test.out|
