#地磁気のlogをxy平面にプロット
#compasslog.txtというファイルが同じディレクトリにあるとする
import matplotlib.pyplot as plt

f = open('compasslog.txt')
xylist = []             #とりあえずcompasslog.txtの値を全て格納
x = []                  #xyの区別がついてないxylistをxとyに分けるためのリスト
y = []
max_x = [-10000]
min_x = [10000]
max_y = [-10000]
min_y = [10000]

def read_log():
    line = f.readline() # 1行を文字列として読み込む(改行文字も含まれる)
    while(line):
        xylist.append(line)
        line = f.readline()
        f.close

def cal():
    num = len(xylist)
    for i in range(int(num/2)):
        x.append(xylist[2*i])
        y.append(xylist[2*i+1])
    for i in range(0, len(x)):
        if(i==0):
            max_x[0] = x[0]
            min_x[0] = x[0]
        else:
            if(x[i] > max_x[0]):
                max_x[0] = x[i]
            elif(x[i] < min_x[0]):
                min_x[0] = x[i]
    for i in range(0, len(y)):
        if(i==0):
            max_y[0] = y[0]
            min_y[0] = y[0]
        else:
            if(y[i] > max_y[0]):
                max_y[0] = y[i]
            elif(y[i] < min_y[0]):
                min_y[0] = y[i]

if __name__ =="__main__":
    read_log()
    cal()
    print("x_offset ={0}".format(max_x[0]))
    print("x_offset ={0}".format(min_x[0]))
    plt.scatter(x, y, marker="o") #散布図作成
    plt.show()
