#地磁気のlogをxy平面にプロット
#compasslog.txtというファイルが同じディレクトリにあるとする
import matplotlib.pyplot as plt

f = open('compasslog.txt')
xylist = []             #とりあえずcompasslog.txtの値を全て格納
x = []                  #xyの区別がついてないxylistをxとyに分けるためのリスト
y = []

def read_log():
    line = f.readline() # 1行を文字列として読み込む(改行文字も含まれる)
    while(line):
        xylist.append(float(line))
        line = f.readline()
        f.close

def get_list():
    num = len(xylist)
    for i in range(int(num/2)):
        x.append(xylist[2*i])
        y.append(xylist[2*i+1])

def cal_max_x():
    max_x = -10000
    for i in range(0, len(x)):
        if(i==0):
            max_x = x[0]
        else:
            if(x[i] > max_x):
                max_x = x[i]
    return max_x


def cal_min_x():
    min_x = 10000
    for i in range(0, len(x)):
        if(i==0):
            min_x = x[0]
        else:
            if(x[i] < min_x):
                min_x = x[i]
    return min_x

def cal_max_y():
    max_y = -10000
    for i in range(0, len(y)):
        if(i==0):
            max_y = y[0]
        else:
            if(y[i] > max_y):
                max_y = y[i]
    return max_y


def cal_min_y():
    min_y = 10000
    for i in range(0, len(y)):
        if(i==0):
            min_y = y[0]
        else:
            if(y[i] < min_y):
                min_y = y[i]
    return min_y



if __name__ =="__main__":
    read_log()
    get_list()
    cal_max_x()
    cal_min_x()
    max_x = cal_max_x()
    min_x = cal_min_x()
    max_y = cal_max_y()
    min_y = cal_min_y()
    x_offset = (max_x + min_x)/2
    y_offset = (max_y + min_y)/2
    print("max_x = {0}\nmin_x = {1}".format(max_x, min_x))
    print("max_y = {0}\nmin_y = {1}".format(max_y, min_y))
    print("x_offset = {0}\ny_offset = {1}".format(x_offset, y_offset))
    plt.scatter(x, y, marker="o") #散布図作成
    plt.show()
