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

num = len(xylist)

for i in range(int(num/2)):
    x.append(xylist[2*i])
    y.append(xylist[2*i+1])

plt.scatter(x, y, marker="o")
plt.show()
