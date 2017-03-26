var messages = [];

function receiveData(event) {
    messages.push(JSON.parse(event.data).message);
    console.log(event.data);
}

var connection = new WebSocket("ws://localhost:26162");

connection.onmessage = receiveData;

export default messages;