// https://github.com/colinbdclark/osc.js/
// https://github.com/romeojeremiah/javascript-light-switch-project/blob/master/style.css

// MESSAGE DISPLAY
let messageText;

// VARIABLES TO BE USED TO POINT TO DOM ELEMENTS
let light;
let photo;
let pot;
let button;


// OSC WEBSOCKET
let webSocketConnected = false;

let socketPort = 8080; 
oscSocket = new osc.WebSocketPort({
				url: "ws://localhost:"+socketPort,
				metadata: true
			});

// ON WEBSOCKET OPEN AND READY
 oscSocket.on("ready", function (msg) {
	console.log("WebSocket Opened on Port "+socketPort);
	messageText.innerText  = "WebSocket Opened on Port "+socketPort;
	webSocketConnected = true;
});

// ON WEBSOCKET MESSAGE
oscSocket.on("message", function (msg) {
	
	let address = msg.address;
	let firstArgumentType = msg.args[0].type;
     let firstArgumentValue = msg.args[0].value;
	
	if ( address == "/pot" ) {

		let angle = (firstArgumentValue/1024.0)*360;
		pot.style.transform = "rotate("+angle+"deg)";

	} else  if ( address == "/photo" ) {

		let size = (firstArgumentValue/1024.0)*200 + 56;
		  photo.style.height = size+'px';
            photo.style.width = size+'px';
		
	} else  if ( address == "/button" ) {

		if ( firstArgumentValue == 0 ) {
			button.src = "button_down.svg";
		} else {
			button.src = "button_up.svg";
		}
		
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
			oscSocket.send({ address: "/led", args: [ { type: "i", value: 255 } ]});
		}
	} else {
		light.src = "lightoff.png";
		if ( webSocketConnected ) {
			oscSocket.send({ address: "/led", args: [ { type: "i", value: 0 } ]});
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

	photo = document.getElementById("photo");
     pot = document.getElementById("pot");
     button = document.getElementById("button");
	
	// CONFIGURE MESSAGE TEXT
	messageText =  document.getElementById("messageText");
	
});




