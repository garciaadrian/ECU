import socket

def main():
    s = socket.socket(
        socket.AF_INET, socket.SOCK_STREAM)
    server_address = ('localhost', 5956)
    print("connecting to {0}".format(server_address))
    s.connect(server_address)
    
    message = "lolu"
    print("sending {0}".format(message))
    s.sendall(message.encode())

    amount_received = 0
    amount_expected = len(message)

    while True:
        data = s.recv(4096)
        if data:
            print("received {0}".format(data))
            print("sending message 'goodbye'")
            s.send('goodbye'.encode())

main()
