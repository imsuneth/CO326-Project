import paho.mqtt.client as mqtt
import time

cl_status = [[0, 0], [0, 0], [0, 0], [0, 0]]

rd_status = [[0, 0], [0, 0], [0, 0], [0, 0]]
rd_status_prev = rd_status

def onCountUpdate(client, userdata, message):
    topic = str(message.topic)
    message=str(message.payload.decode("utf-8"))
    print(topic, message)
    count = int(message)
    r = int(topic[5])
    l = int(topic[-1])
    rd_status[r][l] = count
    #print(rd_status)
    
def cl_publish():
    message=""
    for i in range(4):
        message += str(cl_status[i][0]) + str(cl_status[i][1])
    client.publish("esp32/cl_status", message)
    print('cl_status', cl_status)
    rd_status_prev=rd_status

def cl_stop_all():
    for i in range(4):
            cl_status[i][0] = 0
            cl_status[i][1] = 0

def changed():
    for i in range(4):
        if not rd_status == rd_status_prev:
            return True
    return False

def cl_decide(road):
    if rd_status[road][1]>0:
        cl_status[road][0] = 1
        cl_status[road][1] = 1
        
    elif rd_status[road][0]>0:
        cl_status[road][0] = 1
        #if rd_status[(road+2)%4][0] > 0:
        cl_status[(road+2)%4][0] = 1
        

client = mqtt.Client("raspberrypi")
HOST = "127.0.0.1"
client.connect(HOST, 1883)

for r in range(4):
    for l in range(2):
        client.subscribe("road_" + str(r) + "/lane_" + str(l))

client.on_message = onCountUpdate
client.loop_start()

published = 0
while True:
    
    for road in range(4):
        print("Road :" + str(road))
        print('rd_status', rd_status)
        
        if changed():
            cl_stop_all()
            cl_publish()
        
            
        #if published==0:
            #cl_stop_all()
            #cl_publish()
            #published=1
            #time.sleep(0.01)
        
        if rd_status[road][0]>0 or rd_status[road][1]>0:
            #cl_stop_all()
            cl_decide(road)
            cl_publish()
            published=0
            time.sleep(5)
        
        
