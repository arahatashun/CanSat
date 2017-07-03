#地磁気のlogをxy平面にプロット
#compasslog.txtというファイルが同じディレクトリにあるとする
import matplotlib.pyplot as plt

xylist = []             #とりあえずcompasslog.txtの値を上から全て１つずつ全て格納
x = []                  #xyの区別がついてないxylistをxとyに分けるためのリスト
y = []
x_offset = 0   #このオフセット値を変えてプロットを円に近づける(今の値はEMの値)
y_offset = 0

#log読み取り関数
def read_log():
    txt = open('compasslog.txt')
    for line in txt:
        xylist.append(float(line))
        print(line)
    txt.close

#xとy見境なくリストにしたxylistをxとyにparse
def get_list():
    num = len(xylist)
    for i in range(int(num/2)):
        x.append(xylist[2*i] - x_offset)
        y.append(xylist[2*i+1] - y_offset)

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
    max_x = cal_max_x()
    min_x = cal_min_x()
    max_y = cal_max_y()
    min_y = cal_min_y()
    x_offset = (max_x + min_x)/2
    y_offset = (max_y + min_y)/2
    print("max_x = {0} min_x = {1}".format(max_x, min_x))
    print("max_y = {0} min_y = {1}".format(max_y, min_y))
    print("x_offset = {0} y_offset = {1}".format(x_offset, y_offset))
    plt.scatter(x, y, marker="o") #散布図作成
    plt.axis('equal')
    plt.show()
