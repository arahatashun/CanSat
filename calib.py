import matplotlib.pyplot as plt

f = open('compasslog.txt')
xylist = []
x = []
y = []

line = f.readline() # 1行を文字列として読み込む(改行文字も含まれる)
while(line):
    xylist.append(line)
    line = f.readline()
f.close

num = len(xy)

for i in range(num/2):
    x.append[xy[2i]]
    y.append[xy[2i+1]]

plt.plot(x, y)
plt.show()
