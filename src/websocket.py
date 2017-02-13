import socket

def main():
    s = socket.socket(
        socket.AF_INET, socket.SOCK_STREAM)
    server_address = ('localhost', 26162)
    print("connecting to {0}".format(server_address))
    s.connect(server_address)

    for x in range(0, 10):
        data = s.recv(8192)
        if data:
            print("received {0} {1}/10".format(data, x))
            print("sending message 'goodbye'")
            s.send('goodbye'.encode())

main()
