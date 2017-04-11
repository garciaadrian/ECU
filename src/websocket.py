import asyncio
import websockets
from random import randint
from json import dumps
from time import sleep

def generate_angle():
    return randint(-270, 270)

@asyncio.coroutine
def hello(websocket, path):
    name = yield from websocket.recv()
    print("< {}".format(name))

    while True:
        greeting = dumps({"SteeringWheelAngle": generate_angle(),
                      "type": "telemetry"})
        yield from websocket.send(greeting)
        sleep(0.1)

start_server = websockets.serve(hello, 'localhost', 26162)

asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
