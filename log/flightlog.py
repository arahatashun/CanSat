#!/usr/bin/env python3
# -*- coding: utf-8 -*-

filename1 = "before_light.txt"
filename2 = "during_light.txt"
filename3 = "after_light.txt"

"""
ファイルの書式は以下の通り
filename1(光判定開始前)
diffmin:0.000000
latitude:40.142240 longitude:139.987303 altitude:18.400000
acclx:0.086426 accly:-1.038574 acclz:0.257568
gyrox:0.385254 gyroy:0.204163 gyroz:0.044373
light:58
isLight False
diffmin:0.050000
latitude:40.142233 longitude:139.987288 altitude:18.500000
acclx:0.076904 accly:-1.000000 acclz:0.283691
gyrox:-0.054443 gyroy:0.324890 gyroz:-0.341492
light:10
isLight False

filename2(光判定開始-放出判定)
diffmin:14.250000
latitude:40.142220 longitude:139.986847 altitude:29.400000
acclx:-0.026123 accly:-0.070068 acclz:0.140381
gyrox:1.185303 gyroy:0.165955 gyroz:0.674561
light:37177
isLight True
light_counter:1
diffmin:14.300000
latitude:40.142223 longitude:139.986852 altitude:29.400000
acclx:0.024902 accly:-0.989746 acclz:0.180908
gyrox:0.629211 gyroy:-1.070618 gyroz:0.317627
light:23568
isLight True
light_counter:2

filename3(放出判定-着地判定)
latitude:40.142193 longitude:139.986860 altitude:29.300000
acclx:0.874756 accly:0.050293 acclz:0.591553
gyrox:0.882019 gyroy:0.239624 gyroz:0.126282
Sat Aug 19 03:40:58 2017
latitude:40.142193 longitude:139.986862 altitude:29.300000
acclx:0.929443 accly:-0.008789 acclz:0.578857
gyrox:0.489624 gyroy:-0.003357 gyroz:0.002197
Sat Aug 19 03:41:00 2017
"""

include_whole = 0 #０なら放出判定前含まず、1なら含む

import numpy as np
import matplotlib.pyplot as plt

#route
time = np.array([])
alt = np.array([])
gyrox = np.array([])
gyroy = np.array([])
gyroz = np.array([])
acclx = np.array([])
accly = np.array([])
acclz = np.array([])
light = np.array([])

def before_light():
    global time,alt,gyrox,gyroy,gyroz,acclx,accly,acclz,light
    line_num = -1
    with open(filename1, 'r') as f1:
        for line in f1:
            line_num = line_num + 1
            if line_num % 6 == 0:
                elements = line.rstrip().split(":") 
                for element in elements:
                    if elements.index(element) % 2 == 1: #latitude
                        time = np.append(time,element)

            elif line_num % 6 == 1:
                elements = line.rstrip().split()
                for element in elements:
                    if elements.index(element) % 3 == 2: #lontitude
                        list = element.split(':')
                        alt = np.append(alt,list[1])
                        
            elif line_num % 6 == 2:
                elements = line.rstrip().split()
                for element in elements:
                    if elements.index(element) % 3 == 0: #lontitude
                        list = element.split(":")
                        acclx = np.append(acclx,list[1])
                    if elements.index(element) % 3 == 1: #lontitude
                        list = element.split(":")
                        accly = np.append(accly,list[1])
                    if elements.index(element) % 3 == 2: #lontitude
                        list = element.split(":")
                        acclz = np.append(acclz,list[1])
                        
            elif line_num % 6 == 3:
                elements = line.rstrip().split() 
                for element in elements:
                    if elements.index(element) % 3 == 0: #lontitude
                        list = element.split(":")
                        gyrox = np.append(gyrox,list[1])
                    if elements.index(element) % 3 == 1: #lontitude
                        list = element.split(":")
                        gyroy = np.append(gyroy,list[1])
                    if elements.index(element) % 3 == 2: #lontitude
                        list = element.split(":")
                        gyroz = np.append(gyroz,list[1])
            
            elif line_num % 6 == 4:
                elements = line.rstrip().split(':') 
                for element in elements:
                    if elements.index(element) % 2 == 1: #lontitude
                        light = np.append(light,element)
    

def during_light():
    global time,alt,gyrox,gyroy,gyroz,acclx,accly,acclz,light
    line_num = -1
    with open(filename2, 'r') as f1:
        for line in f1:
            line_num = line_num + 1
            if line_num % 7 == 0:
                elements = line.rstrip().split(":") 
                for element in elements:
                    if elements.index(element) % 2 == 1: #latitude
                        time = np.append(time,element)

            elif line_num % 7 == 1:
                elements = line.rstrip().split()
                for element in elements:
                    if elements.index(element) % 3 == 2: #lontitude
                        list = element.split(':')
                        alt = np.append(alt,list[1])
                        
            elif line_num % 7 == 2:
                elements = line.rstrip().split()
                for element in elements:
                    if elements.index(element) % 3 == 0: #lontitude
                        list = element.split(":")
                        acclx = np.append(acclx,list[1])
                    if elements.index(element) % 3 == 1: #lontitude
                        list = element.split(":")
                        accly = np.append(accly,list[1])
                    if elements.index(element) % 3 == 2: #lontitude
                        list = element.split(":")
                        acclz = np.append(acclz,list[1])
                        
            elif line_num % 7 == 3:
                elements = line.rstrip().split() 
                for element in elements:
                    if elements.index(element) % 3 == 0: #lontitude
                        list = element.split(":")
                        gyrox = np.append(gyrox,list[1])
                    if elements.index(element) % 3 == 1: #lontitude
                        list = element.split(":")
                        gyroy = np.append(gyroy,list[1])
                    if elements.index(element) % 3 == 2: #lontitude
                        list = element.split(":")
                        gyroz = np.append(gyroz,list[1])
            
            elif line_num % 7 == 4:
                elements = line.rstrip().split(':') 
                for element in elements:
                    if elements.index(element) % 2 == 1: #lontitude
                        light = np.append(light,element)
                        
def after_light():
    global time,alt,gyrox,gyroy,gyroz,acclx,accly,acclz,light
    line_num = -1
    counter = 1
    with open(filename3, 'r') as f1:
        for line in f1:
            line_num = line_num + 1
            
            if line_num % 4 == 0:
                elements = line.rstrip().split() 
                for element in elements:
                    if elements.index(element) % 3 == 2:
                        list = element.split(':')
                        alt = np.append(alt,list[1])
                        
            elif line_num % 4 == 1:
                elements = line.rstrip().split()
                for element in elements:
                    if elements.index(element) % 3 == 0: #lontitude
                        list = element.split(":")
                        acclx = np.append(acclx,list[1])
                    if elements.index(element) % 3 == 1: #lontitude
                        list = element.split(":")
                        accly = np.append(accly,list[1])
                    if elements.index(element) % 3 == 2: #lontitude
                        list = element.split(":")
                        acclz = np.append(acclz,list[1])

            elif line_num % 4 == 2:
                elements = line.rstrip().split()
                for element in elements:
                    if elements.index(element) % 3 == 0: #lontitude
                        list = element.split(":")
                        gyrox = np.append(gyrox,list[1])
                    if elements.index(element) % 3 == 1: #lontitude
                        list = element.split(":")
                        gyroy = np.append(gyroy,list[1])
                    if elements.index(element) % 3 == 2: #lontitude
                        list = element.split(":")
                        gyroz = np.append(gyroz,list[1])
                        
            elif line_num % 4 == 3:
                difftime = 14.666 + 0.0333*(counter+1)
                time = np.append(time,difftime)
                counter += 1
                light = np.append(light,0)
            
                
def plot():
    global time,alt,gyrox,gyroy,gyroz,acclx,accly,acclz,light

    # 1. Figureのインスタンスを生成
    fig = plt.figure()
    fig2 = plt.figure()
    fig3 = plt.figure()
    fig4 = plt.figure()
    print("type(fig): {}".format(type(fig)))

    # 2. Axesのインスタンスを生成
    ax1 = fig.add_subplot(111)
    ax2 = fig2.add_subplot(111)
    ax3 = fig3.add_subplot(111)
    ax4 = fig4.add_subplot(111)
    print("type(ax1): {}".format(type(ax1)))

    # 3. データを渡してプロット
    ax1.plot(time,alt, label="altitude")
    ax1.legend()
    
    ax2.plot(time, gyrox, label="gyrox")
    ax2.plot(time, gyroy, label="gyroy")
    ax2.plot(time, gyroz, label="gyroz")
    ax2.legend()
    
    ax3.plot(time, acclx, label="acclx")
    ax3.plot(time, accly, label="accly")
    ax3.plot(time, acclz, label="acclz")
    ax3.legend()
    
    ax4.plot(time,light, label="lux")
    ax4.legend()

    # 4. y軸の範囲を調節とグラフタイトル・ラベル付け
    ax1.set_ylim(0, 40)
    ax2.set_ylim(-1.5, 1.5)
    ax3.set_ylim(-1.5, 1.5)

    ax1.set_title("altitude")
    ax2.set_title("gyro")
    ax3.set_title("accl")
    ax4.set_title("lux")

    ax1.set_xlabel("time")
    ax2.set_xlabel("time")
    ax3.set_xlabel("time")
    ax4.set_xlabel("time")
    
    file_alt = "altitude.png"
    file_gyro = "gyro.png"
    file_accl = "accl.png"
    file_lux = "lux.png"
    fig.savefig(file_alt)
    fig2.savefig(file_gyro)
    fig3.savefig(file_accl)
    fig4.savefig(file_lux)

    fig.tight_layout()  # タイトルとラベルが被るのを解消
    
if __name__ == "__main__":
    if(include_whole == 1):
        before_light()
    during_light()
    after_light()
    plot()


            
