var debugServer = new WebSocket("ws://127.0.0.1:26162");
var domWebsocketStatus = document.getElementById('websocketStatus');
var domLastMessage = document.getElementById('lastMessage');
var domBrake = document.getElementById('brake');

debugServer.onmessage = function (event) {
    console.log(event.data);
    domLastMessage.innerHTML = event.data;
};

debugServer.onerror = function(error) {
    console.log('Websocket error' + error);
};

debugServer.onopen = function (event) {
    domWebsocketStatus.innerHTML = "OPEN";
    console.log("CONNECTION OPEN");
    debugServer.send("hello");
};
