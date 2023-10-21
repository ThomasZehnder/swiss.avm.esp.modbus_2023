tiny-js
=======

(originally [on Google Code](https://code.google.com/p/tiny-js/))

This project aims to be an extremely simple (~2000 line) JavaScript interpreter, meant for 
inclusion in applications that require a simple, familiar script language that can be included
with no dependencies other than normal C++ libraries. It currently consists of two source files:
one containing the interpreter, another containing built-in functions such as String.substring.

TinyJS is not designed to be fast or full-featured. However it is great for scripting simple 
behaviour, or loading & saving settings.

I make absolutely no guarantees that this is compliant to JavaScript/EcmaScript standard. 
In fact I am sure it isn't. However I welcome suggestions for changes that will bring it 
closer to compliance without overly complicating the code, or useful test cases to add to 
the test suite.

Currently TinyJS supports:

* Variables, Arrays, Structures
* JSON parsing and output
* Functions
* Calling C/C++ code from JavaScript
* Objects with Inheritance (not fully implemented)

Please see [CodeExamples](https://github.com/gfwilliams/tiny-js/blob/wiki/CodeExamples.md) for examples of code that works...

For a list of known issues, please see the comments at the top of the TinyJS.cpp file, as well as the [GitHub issues](https://github.com/gfwilliams/tiny-js/issues)

There is also the [42tiny-js branch](https://github.com/gfwilliams/tiny-js/tree/42tiny-js) - this is maintained by Armin and provides a more full-featured JavaScript implementation than GitHub master.

TinyJS is released under an MIT licence.

Internal Structure
------------------------

TinyJS uses a Recursive Descent Parser, so there is no 'Parser Generator' required. It does not
compile to an intermediate code, and instead executes directly from source code. This makes it 
quite fast for code that is executed infrequently, and slow for loops.

Variables, Arrays and Objects are stored in a simple linked list tree structure (42tiny-js uses a C++ Map).
This is simple, but relatively slow for large structures or arrays.

## JavaScript function
--------------------------------
### ESP Interface functions

    Esp.setLed(pixel, color); //set led i[0..3] with color [#rrggbb, off/black, red, green, yellow, blue, white/on]
    Esp.clearLed(); //clear al leds of strip
    Esp.getKey(i); //get key status i[0..2] [0,1]
    Esp.getKeyCounter(i); //get key couter of clicks i[0..2] [int]

    Esp.millis(); // return millis of ESP 
    JS.setCycleTime(time); // set JS cycle time [200..10'000ms]

    Wifi.connected(); //return if Wifi is connected
    Mqtt.connected(); //return if Mqtt is connected

    Mqtt.publish(topic, value); //publish String or Integer under topic. 
    Mqtt.subscribe(topic); //subscribe topic. return value one's first received
    Mqtt.clear();    // clear all subscribed topics.

### Math function
    // --- Math and Trigonometry functions ---
    Math.abs(a)", scMathAbs, 0);
    Math.round(a)", scMathRound, 0);
    Math.min(a,b)", scMathMin, 0);
    Math.max(a,b)", scMathMax, 0);
    Math.range(x,a,b)", scMathRange, 0);
    Math.sign(a)", scMathSign, 0);
    
    Math.pow(a,b)", scMathPow, 0);
    
    Math.sqr(a)", scMathSqr, 0);
    Math.sqrt(a)", scMathSqrt, 0);    
  
### JS Functions
    exec(jsCode); // execute the given code
    eval(jsCode); // execute the given string (an expression) and return the result
    trace();  // 
    Object.dump();
    Object.clone();

    Math.rand()", scMathRand, 0);
    Math.randInt(min, max)", scMathRandInt, 0);

    charToInt(ch); //  convert a character to an int - get its value
    String.indexOf(search); // find the position of a string in a string, -1 if not
    String.substring(lo,hi);
    String.charAt(pos);
    String.charCodeAt(pos);
    String.fromCharCode(char);
    String.split(separator);
    Integer.parseInt(str); // string to int
    Integer.valueOf(str)"; // value of a single character
    //TZE 20230205 unsafe, crashes
    //JSON.stringify(obj, replacer)", scJSONStringify, 0); // convert to JSON. replacer is ignored at the moment
    // JSON.parse is left out as you can (unsafely!) use eval instead
    Array.contains(obj);
    Array.remove(obj);
    Array.join(separator);

### Assembly functions
    Assembly.getProcessState();      // get Assembly process state
    Assembly.setProcessState(state); // set Assembly process state ["new","started","finished","aborted"]

