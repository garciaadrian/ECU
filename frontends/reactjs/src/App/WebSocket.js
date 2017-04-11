var iracingConnection = {"status": 0};
var messages = [];
var telemetry = [null];

function receiveData(event) {
    if (JSON.parse(event.data).type == "log") {
        messages.push(JSON.parse(event.data).message);
        console.log(event.data);
    }

    if (JSON.parse(event.data).status) {
        iracingConnection = JSON.parse(event.data);
        console.log(event.data);
    }

    if (JSON.parse(event.data).type == "telemetry") {
        telemetry = [JSON.parse(event.data)];
        console.log(event.data);
    }
}

var msg = {
    type: "setting",
    setting: "ibt processing",
    value: true
}

function sendData(event) {
    connection.send(JSON.stringify(msg));
}

var connection = new WebSocket("ws://localhost:26162");

connection.onmessage = receiveData;
connection.onopen = sendData;

export default messages;
export var iracingConnection = iracingConnection;
export var telemetry = telemetry;