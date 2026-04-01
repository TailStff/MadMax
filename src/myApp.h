#ifndef MYAPP_H
#define MYAPP_H

#include <DigitalInputs.h>
#include <DigitalOutputs.h>

#include "mmScreenPages.h"

#include "mmPIDProvider.h"
#include "mmMinOnOffProvider.h"
#include "mmDelayOnOffProvider.h"
#include "mmRunTimeProvider.h"
#include "mmTPulseProvider.h"
#include "mmLinearProvider.h"
#include "mmFeedbackErrorProvider.h"
#include "mmDigitalEquipmentProvider.h"
#include "mmPumpSwapProvider.h"
#include "mmVariableProvider.h"
#include "mmAccumProvider.h"

#include "mmFeedbackError.h"

#include "ModbusScheduler.h"
#include "mmModbusServerManager.h"

class MyApp
{

private:
    ExecutionEnv *executionEnv;

    DigitalInputs *digitalInputs;
    DigitalOutputs *digitalOutputs;

    mmModbusServerManager *modbusServerManager;

    ModbusScheduler *scheduler;

    int64_t myTick;

    // Create char buffer for JSON serialization or string concatenation
    char message[128];

    // We store the last time we display something (to limit framerate)
    uint64_t lastMilliDisplay = 0;

    /*
    bool blink = false;
    bool blink2 = false;
    bool blink3 = false;
    bool blink4 = false;

    double measure;
    double pidValue;
    */

    mmPIDProvider *mmPIDs;
    mmMinOnOffProvider *mmMinOnOffs;
    mmDelayOnOffProvider *mmDelayOnOffs;
    mmRunTimeProvider *mmRunTimes;
    mmTPulseProvider *mmTPulses;
    mmLinearProvider *mmLinears;
    mmFeedbackErrorProvider *mmFeedbackErrors;
    mmDigitalEquipmentProvider *mmDigitalEquipments;
    mmPumpSwapProvider *mmPumpSwaps;
    mmVariableProvider *mmVariables;
    mmAccumProvider *mmAccums;

    mmScreenPages *screenPages;

    /*
    mmFeedbackError *feedbackError;


    // Variables globales pour les tâches ModbusRTU
    MBTask t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11;
    */

    // mmDigitalEquipment *digitalEquipment;

    // mmPumpSwap *permut4;

public:
    // Constructors
    MyApp(ExecutionEnv *executionEnv, ModbusClientRTU &MBRTU, mmModbusServerManager *modbusServerManager, DigitalInputs *digitalInputs, DigitalOutputs *digitalOutputs);
    ~MyApp();

    void Init();
    void Loop();

    mmVariableProvider *GetVariableProvider() { return mmVariables; }
    mmAccumProvider *GetAccumsProvider() { return mmAccums; }
    mmDigitalEquipmentProvider *GetDigitalEquipmentProvider() { return mmDigitalEquipments; }
    mmPumpSwapProvider *GetPumpSwapProvider() { return mmPumpSwaps; }
    mmTPulseProvider *GetTPulseProvider() { return mmTPulses; }
    mmFeedbackErrorProvider *GetFeedbackErrorProvider() { return mmFeedbackErrors; }
    mmDelayOnOffProvider *GetDelayOnOffProvider() { return mmDelayOnOffs; }

    void cbShortPress();
    void cbLongPress();
};

#endif