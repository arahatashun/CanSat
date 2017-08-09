# 地磁気キャリブレーション用フォルダ

`make scatter`

でコンパイル.
`sudo stdbuf -o0 -e0 ./scatter.out | tee /home/pi/cansat/compass_calib/compasslog.txt`

で実行し、地磁気のxy軸のraw_dataのcompassslogを作成する.

これを

pythonが使えるコンソール上で同じディレクトリに`compasslog.txt`があるところで

`python calib.py`

calib.py

(compasslog.txtをpythonでparseし、楕円プロットやoffset値などを計算するプログラム)


## 関数の名前一覧

|対象|コンパイル|実行|
|:--|:--|:--|
|compass_scatter.c|make scatter|sudo ./scatter.out|
|calib_test.c|make test|sudo ./test.out|
