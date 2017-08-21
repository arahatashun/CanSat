import csv
import os
import math
from datetime import datetime
import matplotlib.pyplot as plt

alt = []
po = []


def plot_pid_compass(control_time, pid_output, delta_angle):
    plt.plot(control_time, pid_output, label='pid_output', color='g')
    plt.plot(control_time, delta_angle, label='delta_angle', color='b')
    plt.legend()
    plt.xlabel('time[min]')
    plt.ylabel('pid_output & delta_angle[deg]')
    plt.plot(control_time, delta_angle)
    plt.show()


if __name__ == '__main__':
    # カレントディレクトリから.txtファイルだけを選別して辞書を作る
    dir_dict = {}
    dir_list = os.listdir('./')
    for i in range(len(dir_list)):
        if(dir_list[i].count(".txt")):
            dir_dict[i] = dir_list[i]
    for num in dir_dict:
        print("{0}:{1}".format(num, dir_dict[num]))
    got_number = int(
        input("Enter the index of the file you want to analyze: "))
    txt = open(dir_dict[got_number])  # ログtxtファイルを開く
    for line in txt:
        if(line.count('altitude:')):
            lis = line.split(":")
            alt.append(float(lis[3]))
    txt.close

    for i in range(0, len(alt)):
        po.append(i)

    plt.plot(po, alt)
    plt.show()
