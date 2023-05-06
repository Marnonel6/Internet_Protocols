#----- A simple TCP based server program in Python using send() function -----

# import socket

# Create a stream based socket(i.e, a TCP socket)
# operating on IPv4 addressing scheme

# serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# # Bind and listen
# serverSocket.bind(("192.168.1.168", 6000))
# serverSocket.listen()

# # Accept connections
# while True:
#     (clientConnected, clientAddress) = serverSocket.accept()
#     print("Accepted a connection request from %s:%s" % (clientAddress[0], clientAddress[1]))
#     while True:
#         dataFromClient = clientConnected.recv(1024)
#         if not dataFromClient:
#             break
#         print(dataFromClient)
#         print(' '.join(hex(x) for x in dataFromClient))


# NOTE It's important to note that recv() is a blocking function, which means that the program execution
# will be blocked until data is received. If you want to set a timeout for recv() so that the program
# doesn't hang indefinitely, you can use the settimeout() method on the socket object before calling recv().



""" Example parsing code START """

# hex_msg = "0xbb 0x3a 0x3 0x2 0x0 0x0 0x3f 0xd 0xa 0x7e 0x7e 0x8 0x73 0x0 0xef 0x94"

# # Remove any non-hexadecimal characters
# hex_msg = hex_msg.replace(" ", "")
# hex_msg = "".join(filter(lambda x: x in "0123456789abcdefABCDEF", hex_msg))

# # Convert the hex string to bytes
# bytes_msg = bytes.fromhex(hex_msg)

#### BYTE STRINGS FROM RFID SCANNER

# bytes_msg = b'\xbb:\x03\x02\x00\x00?\r\n~~\x08s\x00\xef\x94' # Initial connection
# bytes_msg = b'\xbb@\x02\x00\x01C\r\n~~\x08s\x00\xef\x94' # 0xbb 0x40 0x2 0x0 0x36 0x78 0xd 0xa 0x7e 0x7e 0x8 0x73 0x0 0xef 0x94
bytes_msg = b'\xbb\x17\x130\x00\xe2\x00\x00\x1d%\x03\x02X\x16P\xe7\xa5u\x8d \x1f\x01\x0f\r\n~~\x08s\x00\xef\x94'
# NOTE END1 = 0x0D -> \r
# NOTE END2 = 0x0A -> \n

message = bytes_msg.decode('iso-8859-1')    # convert bytes to string using UTF-8 encoding
print(message)

# bytes_msg = ''.join(hex(x) for x in bytes_msg)

print(bytes_msg)
print(' '.join(hex(x) for x in bytes_msg))

# Parse the message
Head = bytes_msg[0]             # Frame start flag -> 0xBB
Type = bytes_msg[1]             # Instruction Type
Len = bytes_msg[2]              # Number of bytes of Data
# Data = bytes_msg[3:-2]          # Data bytes -> Instruction specifications (from index 3 to second-to-last byte)
Data = bytes_msg[3:int(hex(Len)[2:])+3]
CRC = bytes_msg[int(hex(Len)[2:])+3]  #[-2:]            # Checksum (last two bytes)

# TO go from 0x13 -> 13
# Len = hex(Len)[2:]

# Calculate the checksum of the message
# calculated_checksum = bytes([sum(bytes_msg[1:-2]) & 0xff, sum(bytes_msg[1:-2]) >> 8])
calculated_checksum = sum(bytes_msg[1:int(hex(Len)[2:])+3]) & 0xff

# Compare the calculated checksum with the received checksum
if calculated_checksum == CRC:
    print("Checksum is valid")
else:
    print("Checksum is invalid")

if Type == 0x17:
    print("TAG DATA")

# Print the parsed message
print(f"Start byte: {hex(Head)}")
print(f"Type byte: {hex(Type)}")
print(f"Length byte: {hex(Len)}")
# print(f"Length byte: {Len}")
print(f"Data bytes: {' '.join(hex(x) for x in Data)}")
print(f"Checksum: {hex(CRC)}")
print(f"calculated_checksum: {hex(calculated_checksum)}")


""" Example parsing code END """








# #----- A simple TCP based server program in Python using send() function -----

# import socket

# # Create a stream based socket(i.e, a TCP socket)

# # operating on IPv4 addressing scheme

# serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


# # Bind and listen

# serverSocket.bind(("192.168.1.168",6000))
# serverSocket.listen()


# # Accept connections

# while(True):

#     (clientConnected, clientAddress) = serverSocket.accept()

#     print("Accepted a connection request from %s:%s"%(clientAddress[0], clientAddress[1]))

#     dataFromClient = clientConnected.recv(1024)

#     # print(dataFromClient.decode())
#     # print("Received: %s"%dataFromClient)

#     # data = b'\x48\x65\x6c\x6c\x6f'
#     print(' '.join(hex(x) for x in dataFromClient))

#     # Send some data back to the client

#     # clientConnected.send("Hello Client!".encode())