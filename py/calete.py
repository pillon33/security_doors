import serial
from serial.tools import list_ports
from datetime import datetime
import json


ports = list_ports.comports()
portList = []

for idx, port in enumerate(ports):
    portList.append(port)
    print(f'{str(idx + 1)}.     {str(port)}')

choice = int(input('Choose port to connect...   ')) - 1
portName = str(portList[choice].name)

ser = serial.Serial(portName, 9600)

print( "Successfully connected" )


# 
# flags:
# 0: finished sending data
# 1: should save all data to a file                             (saving data base entries)
# 2: should print all data                                      (printing all cards on whitelist)
# 3: should print some data and write users response            (deleting card from whitelist)
# 

isFirstLine = True
flag = 0

while True:
    if ser.in_waiting:
        if isFirstLine:                             # first line is a flag that defines operation type
            isFirstLine = False
            flag = int( ser.readline().decode() )
        else:
            if flag == 1:
                msg = ser.readline().decode()[:-3]
                try:
                    flag = int(msg)
                except:
                    pass
                filename = f'{datetime.now().strftime("%d_%m_%Y_%H_%M_%S")}_data_base.json'
                small_dict = {}
                big_dict = {}
                while True:
                    msg = ser.readline().decode()[:-2]
                    print(msg)

                    try:
                        flag = int(msg)
                        if flag == 0:
                            break
                    except:
                        pass
                    
                    msg = msg[:-1]
                    tab = msg.split(",")

                    small_dict["card"] = tab[1]
                    small_dict["status"] = tab[2]

                    big_dict[tab[0]] = small_dict
                    small_dict = {}
                content = json.dumps(big_dict)
                with open(filename, 'a') as f:
                    f.write(content)
            elif flag == 2:
                msg = ser.readline().decode()
                try:
                    flag = int(msg)
                except:
                    pass
                while flag != 0:
                    print( msg , end="")
                    msg = ser.readline().decode()
                    try:
                        flag = int(msg)
                    except:
                        pass
            elif flag == 3:
                msg =  ser.readline().decode()
                print( msg )
                if msg == "No cards to remove\r\n":
                    continue
                answer = input()
                ser.write( answer.encode('ascii') )
                print( ser.readline().decode() )
    else:
        isFirstLine = True