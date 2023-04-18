from socket import *
import time

clientSocket = socket(AF_INET, SOCK_DGRAM)
packetCount = 0
packetLoss = 0
totalRTT = 0
# HOST = '127.0.0.1'
HOST = '140.114.89.43'
PORT = 55555
serverAddress = (HOST, PORT)

# clientSocket.settimeout(1.0)

while packetCount < 10:
    startTime = time.time()
    messages = 'PING ' + str(packetCount) + " " + str(time.strftime("%H:%M:%S"))
    clientSocket.settimeout(1.0)
    clientSocket.sendto(messages.encode(), serverAddress)
    try:
        serverData, addr = clientSocket.recvfrom(1024)
        endTime = time.time()
        throughTime = (endTime - startTime) * 1000   # milli-seconds
        totalRTT += throughTime
        print('PING ' + str(packetCount) + ' ' + str(throughTime))
    except timeout:
        packetLoss += 1
        print('Request timed out.')

    packetCount += 1
    

# print('packetLoss: ' + str(packetLoss))
print('Result:')
if packetLoss == 10:
    print('Average RTT ' + str(0))
else:
    print('Average RTT ' + str(totalRTT / (10 - packetLoss)))
print('Packet loss rate ' + str(packetLoss / 10))


