/*******************
 * CONFIGURATION   *
 *******************/
let serialPath = "COM3";
let serialBaud = 115200;
let slipToUdpPort = 8001;
let udpToSlipPort = 8000;


// TODO
/*
// Instantiate a new OSC Serial Port.
var serialPort = new osc.SerialPort({
    devicePath: process.argv[2] || "/dev/tty.usbmodem221361"
});
*/

/*********************
 * CODE FROM HERE ON *
 *********************/
const SerialPort = require('serialport');
let osc = require("osc");

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
    //console.log("From Serial "+oscMessage);
    udpPort.send(oscMessage, "127.0.0.1", slipToUdpPort);
});

serial.on("error", function (errorMsg) {
	console.log(errorMsg);
});


// The open event is always emitted
serial.on("open", function() {
  console.log("Serial port is open!");
    // Open UDP
    console.log("Opening UDP port.");
    udpPort.open();
})



		

/****************
 * OSC Over UDP *
 ****************/

let getIPAddresses = function () {
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
    localPort: udpToSlipPort
});

udpPort.on("ready", function () {
    var ipAddresses = getIPAddresses();

    console.log("Listening for OSC over UDP.");
    ipAddresses.forEach(function (address) {
        console.log(" Host:", address + ", Port:", udpPort.options.localPort);
    });


});

udpPort.on("message", function (oscMessage) {
    console.log("From UDP "+oscMessage);
    serial.send(oscMessage);
});

udpPort.on("error", function (err) {
    console.log(err);
});


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
		console.log("Opening serial port "+serialPath);
		serial.open();
/*
		serial.on("message", function (oscMsg) {
			console.log("An OSC message just arrived!", oscMsg);
        });
*/
		

		
	} else {
		console.log("Serial path \"" +serialPath+"\" not available");
	}

}
