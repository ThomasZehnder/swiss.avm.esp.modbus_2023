// java script cyclic

x = inc(x);

print("x: " + x);
print(Esp.millis());

if (Esp.getKey(0)) {
    Esp.setLed(0, "blue");
}
else {
    Esp.setLed(0, "black");

}

if (x % 2) {
    Esp.setLed(3, "green");
}
else {
    Esp.setLed(3, "black");

}

Esp.setLed(1, "red");
