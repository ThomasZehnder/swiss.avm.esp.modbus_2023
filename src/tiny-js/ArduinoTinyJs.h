#include <ArduinoJson.h>
#include "TinyJS.h"

struct tstJsDisplay
{
    String lines[3];
    bool updateDisplay=true;
};


class ArduinoTinyJs
{
public:

    int loopCounter;

    //JS program parts
    String initStr;
    String cyclicStr;
    String exitStr;

    //error string
    String errorStr;
    bool errorActive;

    //cyclic run flags
    bool setCmd(String cmd);
    void executeCmd(void);
    String cmd;

    bool singleRun;
    bool cyclicRun;

    //Array used for "print"
    #define CONSOLE_LEN 10
    String consoleStr[CONSOLE_LEN];
    const byte consoleStrLength=CONSOLE_LEN;
    int consoleCounter;

    //Display Interface (OLED)
    struct tstJsDisplay display;

    ArduinoTinyJs(); // constructor

    void setup(); //
    void loop();
    void release(); // destroy JS Instance
    void tearDown();

    void loadSetup();
    void loadLoop();
    void loadTearDown();

    //time settinge
    int setCycleTime;
    //time meassurment
    unsigned long startTime;
    unsigned long executeTime;
    unsigned long cycleTime;

private:
    // create instance dynamic
    CTinyJS *js;

    void execute(const String * code, const char * context);
    void load(String * code, const char * filename);

};

extern ArduinoTinyJs tinyJs;
