#include "ecowitt-parser.h"

#include <ArduinoLog.h>
#include <ArduinoJson.h>

void logJson(JsonDocument& doc)
{
    char JSONmessageBuffer[measureJson(doc) + 1];
    serializeJson(doc, JSONmessageBuffer, sizeof(JSONmessageBuffer));
#if defined(setBitrate) || defined(setFreqDev) || defined(setRxBW)
    Log.setShowLevel(false);
    Log.notice(F("."));
    Log.setShowLevel(true);
#else
    Log.notice(F("Received message : %s" CR), JSONmessageBuffer);
#endif
}

void parse_ecowitt(char* message)
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error)
    {
        Log.error(F("Failed to parse JSON: %s" CR), error.c_str());
        return;
    }
    logJson(doc);
}
