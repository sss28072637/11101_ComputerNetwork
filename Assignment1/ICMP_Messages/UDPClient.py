from socket import *
import time
import threading
import struct

ICMP_Message = {
    0: 'net unreachable',
    1: 'host unreachable',
    2: 'destination protocol unreachable',
    3: 'destination port unreachable',
    4: 'fragmentation required, and DF flag set',
    5: 'source route failed',
    6: 'destination network unknown',
    7: 'destination host unknown',
    8: 'source host isolated',
    9: 'network administratively prohibited',
    10: 'host administratively prohibited',
    11: 'network unreachable for TOS',
    12: 'host unreachable for TOS',
    13: 'communication administratively prohibited',
    14: 'host precedence violation',
    15: 'precedence cutoff in effect',
}

udpSocket = socket(AF_INET, SOCK_DGRAM)
icmpSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)

packetCount = 0
packetLoss = 0
totalRTT = 0
# HOST = '127.0.0.1'
HOST = '140.114.89.43'
PORT = 55556
serverAddress = (HOST, PORT)
# icmpSocket.bind(serverAddress)

while packetCount < 10:
    
    startTime = time.time()
    messages = 'HELLO'
    udpSocket.settimeout(1.0)
    udpSocket.sendto(messages.encode(), serverAddress)
    try:
        endTime = time.time()
        throughTime = (endTime - startTime) * 1000   # milli-seconds
        totalRTT += throughTime
    except timeout:
        packetLoss += 1

    packetCount += 1

def listen():
    count = 0
    while count <= 10:
        count += 1
        try:
            mes, ad = icmpSocket.recvfrom(1024)
            icmpSocket.settimeout(1.0)
            icmp_type, code, checksum, packetid, seq = struct.unpack('BbHHh' , mes[20:28])
            print('ICMP Info: type=' + str(icmp_type) + ', ' + 'code=' + str(code) + ', ' + 'message: ' + str(ICMP_Message[icmp_type]))
        except timeout:
            continue
    

icmpThread = threading.Thread(target=listen, args=())
icmpThread.start()
