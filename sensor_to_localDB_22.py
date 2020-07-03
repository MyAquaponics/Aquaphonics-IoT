import paho.mqtt.client as mqtt
import pymysql
import time
import sys
import pymysql.cursors
import time

#ser = serial.Serial('/dev/ttyUSB0',9600)

conn=pymysql.connect(host="localhost", user="root", passwd="vision", db="status_db")

lstV = [0,0,0.0]

def on_connect(client, userData, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("ph")
    client.subscribe("infrared")

def on_message(client,userdata,msg):
    print(msg.topic+"-> "+ msg.payload.decode())
    #res = ser.readline() temp_value = eval(res.decode())

    if msg.topic == "ph":
        lstV[0]=msg.payload.decode()
    elif msg.topic == "infrared":
        lstV[1] = msg.payload.decode()
    else:
        lstV[2] = 3.0 #temp_value

    if (lstV[0]!=0 and lstV[1]!=0) and lstV[2]!=0.0:
        try:
            with conn.cursor() as cur:
                sql = "INSERT INTO status_tb(ph, temp, infrared, date) values (%s,%s,%s,%s)"
                ph = lstV[0]
                infrared = lstV[1]
                temp = lstV[2]
                cur.execute(sql, (ph,temp,infrared, time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())))
                print(ph,temp,infrared, time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
                lstV[0]=0
                lstV[1]=0
                lstV[2]=0.0
                conn.commit()
        except KeyboardInterrupt:
            exit()

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("192.168.0.4",1883,300) #localhost 아닌가?
client.loop_forever()


