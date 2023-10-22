const modbus = require('modbus-stream');
const net = require('net');

const serverIp = '192.168.1.26';
const serverPort = 503;

let toggle = false;

modbus.tcp.connect(serverPort, serverIp, { debug: "automaton-2454" }, (err, client) => {
    // do something with connection

    console.log(err);
    if (err) throw err;

    // Define the Modbus request to toggle Coil 100 (address is 99 because Modbus addresses start at 0)
    const requestCoil = {
        //unit: 1, // Modbus unit ID
        address: 0, // Coil 100 (address is 99 because Modbus addresses start at 0)
        value: 0, // Set the coil to ON
    };

    // Send the request to toggle the coil
    client.writeSingleCoil(requestCoil, (err) => {
        if (err) {
            console.error('Error toggling the coil:', err);
        } else {
            console.log('Coil 100 toggled ON');
        }
    });

    client.readCoils({ address: 100, quantity: 1 }, (err, res) => {
        if (err) throw err;

        console.log(res.state); // response
    })

    // Define the Modbus request to toggle Coil 100 (address is 99 because Modbus addresses start at 0)
    const buf = Buffer.alloc(256);
    const len = buf.write("a");
    const requestRegister = {
        //unit: 1, // Modbus unit ID
        address: 1, // Register 100 (address is 99 because Modbus addresses start at 0)
        value: buf, // Set the coil to ON
    };


    // Send the request to toggle the coil
    client.writeSingleRegister(requestRegister, (err) => {
        if (err) {
            console.error('Error set the reguster:', err);
        } else {
            console.log('Register 100 set');
        }
    });

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
        // Define the Modbus request to toggle Coil 100 (address is 99 because Modbus addresses start at 0)
        const requestCoil = {
            //unit: 1, // Modbus unit ID
            address: 0, // Coil 100 (address is 99 because Modbus addresses start at 0)
            value: toggle, // Set the coil to ON
        };

        // Send the request to toggle the coil
        client.writeSingleCoil(requestCoil, (err) => {
            if (err) {
                console.error('Error toggling the coil:', err);
            } else {
                console.log('Coil 0 toggled ON', toggle);
            }
        });


    }, 1000);

});
