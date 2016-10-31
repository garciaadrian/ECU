import asyncio
import websockets
import socket

@asyncio.coroutine
def hello(websocket, path):
    name = yield from websocket.recv()
    print("< {}".format(name))

    greeting = "Hello {}!".format(name)
    yield from websocket.send(greeting)
    print("> {}".format(greeting))

# start_server = websockets.serve(hello, 'localhost', 8765)

# asyncio.get_event_loop().run_until_complete(start_server)
# asyncio.get_event_loop().run_forever()


def main():
    s = socket.socket(
        socket.AF_INET, socket.SOCK_STREAM)
    server_address = ('localhost', 5956)
    print("connecting to {0}".format(server_address))
    s.connect(server_address)
    
    message = "lolu"
    #print("sending {0}".format(message))
    #s.sendall(message.encode())

    amount_received = 0
    amount_expected = len(message)

    while True:
        data = s.recv(4096)
        if data:
            print("received {0}".format(data))
            print("sending message 'goodbye'")
            s.send('goodbye'.encode())

main()
