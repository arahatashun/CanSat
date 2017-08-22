#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
plt.rcParams['animation.ffmpeg_path'] = '/usr/local/bin/ffmpeg'

#filename
filename1 = "file1.txt"
filename2 = "file2.txt"
#ファイルの書式は以下の通り
"""
latitude:40.019077
longitude:139.957732
altitude:31.100000
latitude:40.019078
longitude:139.957732
altitude:31.100000
latitude:40.019078
longitude:139.957732
"""
#route
x1 = np.array([])
x2 = np.array([])
y1 = np.array([])
y2 = np.array([])
#route display
x1_s = np.array([])
y1_s = np.array([])
x2_s = np.array([])
y2_s = np.array([])

fig = plt.figure(1)
ax = fig.add_subplot(111) 
ax.set_ylim([40.017, 40.020])
ax.set_xlim([139.957, 139.958])
ims = []
im, = ax.plot(x1_s,y1_s,'b',label = "kondo")
im2, = ax.plot(x2_s,y2_s,'g',label = "arahata")


def read_file():
    global x1,y1,x2,y2,im,im2
    line_num = 0
    with open('file1.txt', 'r') as f1:
        for line in f1:
            line_num = line_num + 1
            if line_num % 3 == 2:
                elements = line.rstrip().split(':') 
                for element in elements:
                    if elements.index(element) % 2 == 1: #latitude
                        x1 = np.append(x1,element)

            if line_num % 3 == 1:
                elements = line.rstrip().split(':') 
                for element in elements:
                    if elements.index(element) % 2 == 1: #lontitude
                        y1 = np.append(y1,element)
    line_num = 0                 
    with open('file2.txt', 'r') as f2:
        for line in f2:
            line_num = line_num + 1
            if line_num % 3 == 2:
                elements = line.rstrip().split(':') 
                for element in elements:
                    if elements.index(element) % 2 == 1: #latitude
                        x2 = np.append(x2,element)

            if line_num % 3 == 1:
                elements = line.rstrip().split(':') 
                for element in elements:
                    if elements.index(element) % 2 == 1: #lontitude
                        y2 = np.append(y2,element) 
                        
    print(x1,len(x1))
    print(x2,len(x2))
    print(y1,len(y1))
    print(y2,len(y2))

def update_anim(i):
    global x1_s,y1_s,x2_s,y2_s,im,im2  
    if(i < len(x1)):
        x1_s = np.append(x1_s,x1[i]) 
        y1_s = np.append(y1_s,y1[i])
    if(i < len(x2)):
        x2_s = np.append(x2_s,x2[i]) 
        y2_s = np.append(y2_s,y2[i])

    #im = ax.plot(x1_s,y1_s,'b',label = "kondo")
    #im2 = ax.plot(x2_s,y2_s,'g',label = "arahata")
    #ims.append(im)
    #ims.append(im2)
    im.set_data(x1_s,y1_s)
    im2.set_data(x2_s,y2_s)
    if(i==0):
        plt.legend()
    print(i)
    
if __name__ == "__main__":
    read_file()
    anim = FuncAnimation(fig,update_anim,interval=200,frames = 533)
    anim.save('anim.mp4', writer='ffmpeg')

           


            
