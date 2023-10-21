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

Esp.setLed(1, "red");