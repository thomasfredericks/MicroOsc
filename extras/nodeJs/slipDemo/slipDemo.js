/*******************
 * CONFIGURATION   *
 *******************/
let serialPath = "COM7";
let serialBaud = 115200;
let wsPort = 8080;

/*********************
 * CODE FROM HERE ON *
 *********************/
const SerialPort = require('serialport');
let osc = require("osc"),
    express = require("express"),
    WebSocket = require("ws");


/*******************
 * OSC Over Serial *
 *******************/

// Instantiate a new OSC Serial Port.
let serial = new osc.SerialPort({
    devicePath: serialPath,
	bitrate : serialBaud,
    metadata: true	
});
/*
serial.on("message", function (oscMessage) {
  

});
*/
serial.on("error", function (errorMsg) {
	console.log("The was a SLIP parsing error. Continuing.");
	//console.log(errorMsg);
});



let serialPaths = [];



// List serial ports and then start
SerialPort.list().then(ports => {
	console.log("\n******************");
    console.log("*****STARTING*****");
	console.log("******************");

   console.log("\nConfiguration:");
	console.log("* Serial Port: "+serialPath);
	console.log("* Serial Baud: "+serialBaud);
	console.log("* WebSocket Port: "+wsPort);
	
	console.log("\nAvailable serial ports:");
	ports.forEach(function(port) {
		console.log("-----------------------");
		console.log("Name: "+port.path);
		console.log("Id: "+port.pnpId);
		console.log("Manufacturer: "+port.manufacturer);
		
		serialPaths.push(port.path);
	});
	console.log("-----------------------");
	start();
});

let appResources;
let socketPort;
let relay;

function start() {
	if ( serialPaths.includes(serialPath) ) {		
		// Open serial port.
		console.log("\nOpening Serial Port "+serialPath);
		serial.open();
/*
		serial.on("message", function (oscMsg) {
			console.log("An OSC message just arrived!", oscMsg);
        });
*/
		// Open UDP
		//udpPort.open();
		// Create an Express-based Web Socket server to which OSC messages will be relayed.
		console.log("\nStarting Web Server Listening On Port "+wsPort);
		appResources = __dirname + "/web",
			app = express(),
			server = app.listen(wsPort),
			wss = new WebSocket.Server({
				server: server
			});

		app.use("/", express.static(appResources));
		wss.on("connection", function (socket) {
			console.log("A Web Socket connection has been established!");
			socketPort = new osc.WebSocketPort({
				socket: socket,
				metadata: true	
			});

			relay = new osc.Relay(serial, socketPort);
		});
		

	} else {
		console.log("\nConfigured serial port \"" +serialPath+"\" not available");
	}

}
