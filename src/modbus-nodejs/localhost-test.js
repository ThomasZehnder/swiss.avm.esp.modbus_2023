const modbus = require('modbus-stream');
const net = require('net');

const serverIp = '192.168.1.26';
const serverPort = 503;

let toggle = false;
let counter = 256;

modbus.tcp.connect(serverPort, serverIp, { debug: "automaton-2454" }, (err, client) => {
    // do something with connection

    console.log(err);
    if (err) throw err;


    // Handle errors during the connection
    client.on('error', (err) => {
        console.error('Error connecting to Modbus server:', err);
    });

    // Handle the connection close event
    client.on('close', () => {
        console.log('Connection closed');
    });

    console.log("connected")

    setInterval(() => {
        toggle = !toggle;

        counter++;

        // Define the Modbus request to toggle Coil 0
        const requestCoil = {
            //unit: 1, // Modbus unit ID
            address: 0, // Coil 0
            value: toggle, // Set the coil to toggle
        };

        // Send the request to toggle the coil
        client.writeSingleCoil(requestCoil, (err) => {
            if (err) {
                console.error('Error toggling the coil:', err);
            } else {
                console.log('Coil 0 toggled: ', toggle);
            }
        });

        const buf = Buffer.alloc(256);
        const len = buf.writeUInt16BE(counter);
        const requestRegister = {
            //unit: 1, // Modbus unit ID
            address: 1, // Register 1
            value: buf, // Set register
        }

 
        // Send the request to toggle the coil
        client.writeSingleRegister(requestRegister, (err) => {
            if (err) {
                console.error('Error set the register:', err);
            } else {
                console.log('Register 1 set', counter);
            }
        });

    }, 1000);

});
