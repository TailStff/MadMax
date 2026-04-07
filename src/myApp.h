#ifndef MYAPP_H
#define MYAPP_H

#include <DigitalInputs.h>
#include <DigitalOutputs.h>

#include "mmScreenPages.h"

#include "PIDProvider.h"
#include "MinOnOffProvider.h"
#include "DelayOnOffProvider.h"
#include "RunTimeProvider.h"
#include "TPulseProvider.h"
#include "LinearProvider.h"
#include "FeedbackErrorProvider.h"
#include "FeedbackError.h"
#include "DigitalEquipmentProvider.h"
#include "PumpSwapProvider.h"
#include "VariableProvider.h"
#include "AccumProvider.h"

#include "ModbusScheduler.h"
#include "ModbusServerMemoryManager.h"

class MyApp
{

private:
    ExecutionEnv *executionEnv;

    DigitalInputs *digitalInputs;
    DigitalOutputs *digitalOutputs;

    MadMax::ModbusServerMemoryManager *modbusServerManager;

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

    MadMax::PIDProvider *mmPIDs;
    MadMax::MinOnOffProvider *mmMinOnOffs;
    MadMax::DelayOnOffProvider *mmDelayOnOffs;
    MadMax::RunTimeProvider *mmRunTimes;
    MadMax::TPulseProvider *mmTPulses;
    MadMax::LinearProvider *mmLinears;
    MadMax::FeedbackErrorProvider *mmFeedbackErrors;
    MadMax::DigitalEquipmentProvider *mmDigitalEquipments;
    MadMax::PumpSwapProvider *mmPumpSwaps;
    MadMax::VariableProvider *mmVariables;
    MadMax::AccumProvider *mmAccums;

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
    MyApp(ExecutionEnv *executionEnv, ModbusClientRTU &MBRTU, MadMax::ModbusServerMemoryManager *modbusServerManager, DigitalInputs *digitalInputs, DigitalOutputs *digitalOutputs);
    ~MyApp();

    void Init();
    void Loop();

    MadMax::VariableProvider *GetVariableProvider() { return mmVariables; }
    MadMax::AccumProvider *GetAccumsProvider() { return mmAccums; }
    MadMax::DigitalEquipmentProvider *GetDigitalEquipmentProvider() { return mmDigitalEquipments; }
    MadMax::PumpSwapProvider *GetPumpSwapProvider() { return mmPumpSwaps; }
    MadMax::TPulseProvider *GetTPulseProvider() { return mmTPulses; }
    MadMax::FeedbackErrorProvider *GetFeedbackErrorProvider() { return mmFeedbackErrors; }
    MadMax::DelayOnOffProvider *GetDelayOnOffProvider() { return mmDelayOnOffs; }
    MadMax::RunTimeProvider *GetRunTimeProvider() { return mmRunTimes; }

    void cbShortPress();
    void cbLongPress();
};

#endif