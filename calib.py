#地磁気のlogをxy平面にプロット
#compasslog.txtというファイルが同じディレクトリにあるとする
import matplotlib.pyplot as plt

f = open('compasslog.txt')
xylist = []             #とりあえずcompasslog.txtの値を全て格納
x = []                  #xyの区別がついてないxylistをxとyに分けるためのリスト
y = []

line = f.readline() # 1行を文字列として読み込む(改行文字も含まれる)
while(line):
    xylist.append(line)
    line = f.readline()
f.close

num = len(xylist)

for i in range(int(num/2)):
    x.append(xylist[2*i])
    y.append(xylist[2*i+1])

plt.scatter(x, y, marker="o") #散布図作成
plt.show()
