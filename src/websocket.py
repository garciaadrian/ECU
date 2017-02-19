import asyncio
import websockets

# def main():
#     s = socket.socket(
#         socket.AF_INET, socket.SOCK_STREAM)
#     server_address = ('localhost', 26162)
#     print("connecting to {0}".format(server_address))
#     s.connect(server_address)

#     for x in range(0, 10):
#         data = s.recv(8192)
#         if data:
#             print("received {0} {1}/10".format(data, x))
#             print("sending message 'goodbye'")
#             s.send('goodbye'.encode())

# main()

@asyncio.coroutine
def hello():
    websocket = yield from websockets.connect('ws://localhost:26162')

    try:
        name = 'What is your name?'
        yield from websocket.send()
        print("> {}".format(name))

        greeting = yield from websocket.recv()
        print("< {}".format(greeting))

    finally:
        yield from websocket.close()

asyncio.get_event_loop().run_until_complete(hello())
