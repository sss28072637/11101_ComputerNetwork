from socket import *
import time

HOST = '140.114.89.43'
# HOST = gethostbyname('baidu.com')
PORT =  33434
MESSAGE = 'HELLO'
ADDR = (HOST, PORT)

router_num = 1
hop = 0
maxHop = 30

while hop < maxHop:
    # print('hop = {}'.format(hop))
    udpSocket = socket(AF_INET, SOCK_DGRAM)
    udpSocket.setsockopt(SOL_IP, IP_TTL, router_num)
    icmpSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)
    icmpSocket.bind(('', PORT))
    curName = ''
    curAddr = ''

    # icmpSocket.settimeout(1.0)
    # startTime = time.time()
    # udpSocket.sendto(MESSAGE.encode(), ADDR)
    times = 0
    done = False
    connected = False
    loss_times = 0
    out_str = ''
    hop += 1
    print(str(router_num) , end='   ')
    while True: #try 3 times
        times += 1
        try:
            startTime = time.time()
            udpSocket.sendto(MESSAGE.encode(), ADDR)
            icmpSocket.settimeout(1.0)
            mes, addr = icmpSocket.recvfrom(1024)
            endTime = time.time()
            # if times == 1:
            if connected == False:
                try:
                    curAddr = addr[0]
                    curName = gethostbyaddr(curAddr)
                    curName = curName[0]
                except herror:
                    curName = addr[0]
                out_str += str(curName) + ' '
                out_str += '(' + str(curAddr) +') '
                connected = True
            rtt = (endTime - startTime) * 1000
            rtt = round(rtt, 5)
            if times == 3:
                out_str += (str(rtt) + ' ms')
            else:
                out_str += (str(rtt) + ' ms ')
            done = True
        except timeout:
            loss_times += 1
            if times == 3:
                out_str += '*'
            else:
                out_str += '* '

        if times == 3:
            break
    
    # if done == False:
    #     print('Request timed out.')
    # else:
    #     print(out_str)
    if done == False:
        out_str += ' Request timed out.'
        
    print(out_str)

    router_num += 1
    

    icmpSocket.close()
    udpSocket.close()

    if str(curAddr) == str(HOST):
        break
