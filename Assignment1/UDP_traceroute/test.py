import threading
from urllib.error import URLError
from socket import *
from time import time
import struct
ICMP__MESSAGE = \
    {0: {0: 'echo reply',
        },
     3: {0: 'net unreachable',
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
         14: 'host Precedence violation',
         15: 'precedence cutoff in effect',
        },
     4: {0: 'source quench',
        },
     5: {0: 'redirect datagram for the network',
         1: 'redirect datagram for the host',
         2: 'redirect datagram for the TOS & network',
         3: 'redirect datagram for the TOS & host',
        },
     8: {0: 'echo request',
        },
     9: {0: 'router advertisement',
        },
     10: {0: 'router discovery/selection/solicitation',
         },
     11: {0: 'TTL expired in transit',
          1: 'fragment reassembly time exceeded',
         },
     12: {0: 'pointer indicates the error',
          1: 'missing a required option',
          2: 'bad length',
         },
     13: {0: 'timestamp',
         },
     14: {0: 'timestamp reply',
         },
    }

def threading_funtcion():
    # print('new thread')
    icmp_clientSocket = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP)
    icmp_clientSocket.setsockopt(IPPROTO_IP, IP_HDRINCL, 1)
    # icmp_clientSocket.setsockopt(SOL_IP, IP_HDRINCL, 1)
    icmp_clientSocket.settimeout(1.0)
    while sequence_number!=11:
        try:
            data, addr = icmp_clientSocket.recvfrom(1024)
            # print('rev data')
            icmpHeader=data[20:28]
            icmp_type, code, checksum, packetid, seq = struct.unpack('BbHHh', icmpHeader)
            # print('ICMP Info: type='+str(icmp_type)+', code='+str(code)+', message: '+ str(data))
            # print(str(ICMP__MESSAGE[icmp_type][code]))
            print('ICMP Info: type='+str(icmp_type)+', code='+str(code)+', message: '+str(ICMP__MESSAGE[icmp_type][code])+'.')
            # break
        except timeout:
            continue
            # continue
            # print('timeout')
        # continue

# if name == 'main':
#Create a UDP socket
clientSocket = socket(AF_INET,SOCK_DGRAM)
clientSocket.settimeout(1.0)

sequence_number = 1
thread1 = threading.Thread(target=threading_funtcion,args=())
thread1.start()
# print("check1")
while True:
    # print("check2")
    if sequence_number == 11:
        # print('seq_num = 10')
        break  
    send_time = time()
    # print("check send time")
    send_msg = 'ping '+str(sequence_number)+' '+str(send_time*1000)
    # clientSocket.sendto(send_msg.encode("utf-8"),('127.0.0.1', 55556))
    clientSocket.sendto(send_msg.encode("utf-8"),('140.114.89.43', 55556))
    # print("check3")
    try:
        message,address = clientSocket.recvfrom(1024)
        recv_time = time()
        sequence_number += 1
        # print('clint recv')
        # print ('Server recieved from {}:{}'.format(address,message.decode('utf-8')))      
        # print ("PING "+str(sequence_number)+" "+str(round_trip_time))
        # print(message.decode("utf-8"))
    except timeout:
        sequence_number += 1
        # print("Request timed out.")
    # sequence_number += 1
    # print('seq plus')
      
# sequence_number += 1
# continue

clientSocket.close()
# icmp_clientSocket.close()