// https://github.com/colinbdclark/osc.js/
// https://github.com/romeojeremiah/javascript-light-switch-project/blob/master/style.css

// MESSAGE DISPLAY
let messageText;

// LIGHT
let light;

// OSC WEBSOCKET
let webSocketConnected = false;

oscSocket = new osc.WebSocketPort({
				url: "ws://localhost:8081",
				metadata: true
			});

// ON WEBSOCKET OPEN AND READY
 oscSocket.on("ready", function (msg) {
	console.log("WebSocket opened");
	messageText.innerText  = "WebSocket opened";
	webSocketConnected = true;
});

// ON WEBSOCKET MESSAGE
oscSocket.on("message", function (msg) {
	//console.log("message", msg);
	let address = msg.address;
	let firstArgumentType = msg.args[0].type;
    let firstArgumentValue = msg.args[0].value;
	
	if ( address == "/ms" ) {
		messageText.innerText  = "/ms "+firstArgumentValue;
	}
});

// ON WEBSOCKET CLOSED
oscSocket.on("close", function (msg) {
	console.log("WebSocket closed");
	messageText.innerText  = "WebSocket closed";
	webSocketConnected = false;
});


// TOGGLE LIGHT ON CLICK
function toggleLight() {
	console.log("Light was clicked!");
	if (light.src.match("lightoff")) {
		light.src = "lighton.png";
		if ( webSocketConnected ) {
			oscSocket.send({ address: "/debugLed", args: [ { type: "i", value: 1 } ]});
		}
	} else {
		light.src = "lightoff.png";
		if ( webSocketConnected ) {
			oscSocket.send({ address: "/debugLed", args: [ { type: "i", value: 0 } ]});
		}
	}
}


// ON WINDOW UNLOAD
window.addEventListener("beforeunload", (event) => {
     oscSocket.close();
});

// ON WINDOW LOAD
window.addEventListener('load', (event) => {
     oscSocket.open();
	 
	 // CONFIGURE LIGHT
	light = document.getElementById("light");
	light.addEventListener("mousedown", toggleLight);
	
	// CONFIGURE MESSAGE TEXT
	messageText =  document.getElementById("messageText");
	
});




