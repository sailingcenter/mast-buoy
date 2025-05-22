#include "rtl-433-receiver.h"
#include "ecowitt-parser.h"

#include <rtl_433_ESP.h>

#ifndef RF_MODULE_FREQUENCY
#  define RF_MODULE_FREQUENCY 433.92
#endif

#if defined(setBitrate) || defined(setFreqDev) || defined(setRxBW)

#  ifdef setBitrate
#    define TEST    "setBitrate" // 17.24 was suggested
#    define STEP    2
#    define stepMin 1
#    define stepMax 300
// #    define STEP    1
// #    define stepMin 133
// #    define stepMax 138
#  elif defined(setFreqDev) // 40 kHz was suggested
#    define TEST    "setFrequencyDeviation"
#    define STEP    1
#    define stepMin 5
#    define stepMax 200
#  elif defined(setRxBW)
#    define TEST "setRxBandwidth"

#    ifdef defined(RF_SX1276) || defined(RF_SX1278)
#      define STEP    5
#      define stepMin 5
#      define stepMax 250
#    else
#      define STEP    5
#      define stepMin 58
#      define stepMax 812
// #      define STEP    0.01
// #      define stepMin 202.00
// #      define stepMax 205.00
#    endif
#  endif
float step = stepMin;
#endif

unsigned long uptime()
{
    static unsigned long lastUptime = 0;
    static unsigned long uptimeAdd = 0;
    unsigned long uptime = millis() / 1000 + uptimeAdd;
    if (uptime < lastUptime)
    {
        uptime += 4294967;
        uptimeAdd += 4294967;
    }
    lastUptime = uptime;
    return uptime;
}

unsigned long next = uptime() + 30;


rtl_433_ESP rf; // use -1 to disable transmitter
int count = 0;

#define JSON_MSG_BUFFER 512
char messageBuffer[JSON_MSG_BUFFER];

void rtl_433_Callback(char* message)
{
    parse_ecowitt(message);
    count++;
}

void rtl_433_receiver_init()
{
    rtl_433_ESP::initReceiver(RF_MODULE_RECEIVER_GPIO, RF_MODULE_FREQUENCY);
    rf.setCallback(rtl_433_Callback, messageBuffer, JSON_MSG_BUFFER);
    rtl_433_ESP::enableReceiver();
    rtl_433_ESP::getModuleStatus();
}

void rtl_433_receiver_loop()
{
    rf.loop();
#if defined(setBitrate) || defined(setFreqDev) || defined(setRxBW)
    char stepPrint[8];
    if (uptime() > next) {
        next = uptime() + 120; // 60 seconds
        dtostrf(step, 7, 2, stepPrint);
        Log.notice(F(CR "Finished %s: %s, count: %d" CR), TEST, stepPrint, count);
        step += STEP;
        if (step > stepMax) {
            step = stepMin;
        }
        dtostrf(step, 7, 2, stepPrint);
        Log.notice(F("Starting %s with %s" CR), TEST, stepPrint);
        count = 0;

        int16_t state = 0;
#  ifdef setBitrate
        state = rf.setBitRate(step);
        RADIOLIB_STATE(state, TEST);
#  elif defined(setFreqDev)
        state = rf.setFrequencyDeviation(step);
        RADIOLIB_STATE(state, TEST);
#  elif defined(setRxBW)
        state = rf.setRxBandwidth(step);
        if ((state) != RADIOLIB_ERR_NONE) {
            Log.notice(F(CR "Setting  %s: to %s, failed" CR), TEST, stepPrint);
            next = uptime() - 1;
        }
#  endif

        rf.receiveDirect();
        // rf.getModuleStatus();
    }
#endif
}
