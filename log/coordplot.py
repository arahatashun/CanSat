import csv
import os
import math
import matplotlib.pyplot as plt

#緯度経度たちを便宜上xyz座標に変換してそれをリストで返す
def latlng_to_xyz(lat,long):
    cartesian_coord = []
    rlat = lat*math.pi/180
    rlng = lat*math.pi/180
    coslat = math.cos(rlat)
    cartesian_coord.append(coslat*math.cos(rlng))#x座標
    cartesian_coord.append(coslat*math.sin(rlng))#y座標
    cartesian_coord.append(math.sin(rlat))#z座標
    return cartesian_coord

#２地点のxyz座標のリストからその間の距離を返す
def dist_on_sphere(start_xyz,end_xyz):
    EARTH_RADIUS = 6378137
    dot_product_x = start_xyz[0]*end_xyz[0]
    dot_product_y = start_xyz[1]*end_xyz[1]
    dot_product_z = start_xyz[2]*end_xyz[2]
    dot_product_sum =dot_product_x+dot_product_y+dot_product_z
    distance = abs(math.acos(dot_product_sum)*EARTH_RADIUS)
    return distance

#緯度経度の多次元配列から軌跡をプロット
def plot_coordinate(latlong_coord):
    plt.plot(latlong_coord[1], latlong_coord[0])
    plt.plot(latlong_coord[1][0], latlong_coord[0][0],color = 'k', marker="$START$",markersize=50)
    plt.plot(latlong_coord[1][len(latlong_coord[0])-1], latlong_coord[0][len(latlong_coord[1])-1],color = 'k', marker="$GOAL$",markersize=50)
    plt.gca().get_xaxis().get_major_formatter().set_useOffset(False)
    plt.gca().get_yaxis().get_major_formatter().set_useOffset(False)
    plt.xlabel('longitude')
    plt.ylabel('latitude')
    plt.show()

if __name__ =='__main__':
    #カレントディレクトリから.txtファイルだけを選別して辞書を作る
    dir_dict = {}
    dir_list = os.listdir('./')
    for i in range(len(dir_list)):
        if(dir_list[i].count(".txt")):
            dir_dict[i] = dir_list[i]
    for num in dir_dict:
        print("{0}:{1}".format(num, dir_dict[num]))
    got_number = int(input("Enter the index of the file you want to analyze: "))

    latlong_coord = [[],[]]##緯度経度を多次元配列に格納するためのリストを用意
    dist = []##距離を格納するリストを用意
    time = []##日時を格納するリストを用意

    txt = open(dir_dict[got_number])##ログtxtファイルを開く
    with open('gmplot.csv', 'w') as csvfile: #GPS visualizerのための緯度経度csvファイルを用意
        writer = csv.writer(csvfile, lineterminator='\n')
        writer.writerow(['latitude', 'longitude'])
        csvlist = [0,0]
        for line in txt:
            if(line.count('distance :')):
                lis = line.split(":")
                dist.append(float(lis[1]))
            elif(line.count('latitude:')):
                lis = line.split(":")
                csvlist[0] = float(lis[1])
                latlong_coord[0].append(round(float(lis[1]),6))
            elif(line.count('longitude:')):
                lis = line.split(":")
                csvlist[1] = float(lis[1])
                latlong_coord[1].append(round(float(lis[1]),5))
                writer.writerow(csvlist)
            elif(line.count('2017')):
                time.append(line)

    txt.close
    print("control start time(GBT) is {0}".format(time[0]))
    print("control end time(GBT) is {0}".format(time[len(time)-1]))
    print("distance from control start point to goal is {0}[m]\n".format(round(dist[0],4)))
    print("distance from control end point to goal is {0}[m]\n".format(round(dist[len(dist)-1],4)))
    start_xyz = latlng_to_xyz(latlong_coord[0][0],latlong_coord[1][0]) #制御開始時の緯度経度から便宜上のxyz座標を計算
    end_xyz = latlng_to_xyz(latlong_coord[0][len(latlong_coord[0])-1],latlong_coord[1][len(latlong_coord[1])-1])#制御終了時の緯度経度から便宜上のxyz座標を計算
    print("distance from control start point to control end point is {0}[m]\n".format
    (round(dist_on_sphere(start_xyz,end_xyz),4)))

    plot_coordinate(latlong_coord)
