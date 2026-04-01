/*#pragma once

#include <vector>
#include <cstdint>
#include <ArduinoJson.h>
#include "mmDigitalEquipmentStatus.h"

// Spécialisation ArduinoJson pour mmBoolArray
namespace ArduinoJson
{
    template <>
    struct Converter<DigitalEquipmentStatus>
    {
        static void toJson(const DigitalEquipmentStatus &src, JsonVariant dst)
        {
            JsonObject obj = dst.to<JsonObject>();
            obj["command"] = src.command;
            obj["fault"] = src.fault;
            obj["feedbackFault"] = src.feedbackFault;
            obj["feedback"] = src.feedback;
            obj["output"] = src.output;
            obj["runTime"] = src.runTimeValue;
            obj["startCount"] = src.startCountValue;
        }
    };
}*/