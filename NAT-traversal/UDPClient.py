import socket
import sys
import time

HOST, PORT = sys.argv[1], 9999
data = "hello"

# SOCK_DGRAM is the socket type to use for UDP sockets
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.sendto(data + "\n", (HOST, PORT))

# As you can see, there is no connect() call; UDP has no connections.
# Instead, data is directly sent to the recipient via sendto().
while True:
    received = sock.recv(1024)
    print "{}".format(received)
    time.sleep(1)
    

