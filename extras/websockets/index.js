/*******************
 * CONFIGURATION   *
 *******************/
let serialPath = "COM3";
let serialBaud = 38400;
let wsPort = 8081;

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

serial.on("message", function (oscMessage) {
    console.log(oscMessage);
});

serial.on("error", function (errorMsg) {
	console.log(errorMsg);
});


/****************
 * OSC Over UDP *
 ****************/
/*
var getIPAddresses = function () {
    var os = require("os"),
        interfaces = os.networkInterfaces(),
        ipAddresses = [];

    for (var deviceName in interfaces) {
        var addresses = interfaces[deviceName];
        for (var i = 0; i < addresses.length; i++) {
            var addressInfo = addresses[i];
            if (addressInfo.family === "IPv4" && !addressInfo.internal) {
                ipAddresses.push(addressInfo.address);
            }
        }
    }

    return ipAddresses;
};

// Bind to a UDP socket to listen for incoming OSC events.
var udpPort = new osc.UDPPort({
    localAddress: "0.0.0.0",
    localPort: 57121
});

udpPort.on("ready", function () {
    var ipAddresses = getIPAddresses();

    console.log("Listening for OSC over UDP.");
    ipAddresses.forEach(function (address) {
        console.log(" Host:", address + ", Port:", udpPort.options.localPort);
    });
});

udpPort.on("message", function (oscMessage) {
    console.log(oscMessage);
});

udpPort.on("error", function (err) {
    console.log(err);
});
*/

let serialPaths = [];



// List serial ports and then start
SerialPort.list().then(ports => {
	console.log("Available serial ports:");
	ports.forEach(function(port) {
		console.log("---");
		console.log("Path: "+port.path);
		console.log("Id: "+port.pnpId);
		console.log("Manufacturer: "+port.manufacturer);
		console.log("---");
		serialPaths.push(port.path);
	});
	start();
});

let appResources;
let socketPort;
let relay;

function start() {
	if ( serialPaths.includes(serialPath) ) {		
		// Open serial port.
		console.log("Opening serial path "+serialPath);
		serial.open();
		serial.on("message", function (oscMsg) {
			console.log("An OSC message just arrived!", oscMsg);
        });
		// Open UDP
		//udpPort.open();
		// Create an Express-based Web Socket server to which OSC messages will be relayed.
		console.log("Starting Web server listening on "+wsPort);
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

			relay = new osc.Relay(serial, socketPort, {
				raw: true
			});
		});
		

	} else {
		console.log("Serial path \"" +serialPath+"\" not available");
	}

}
