// #define SERIALDEBUG

#include "myApp.h"

#include <ModbusClientRTU.h>
#include <HardwareSerial.h>

#define GPIO0 0
#define DISPLAYPERIOD 0

void MyApp::cbShortPress()
{
#ifdef SERIALDEBUG
    Serial.println("short press");
#endif
    screenPages->IncPage();
    Serial.println("!short press");
}

void MyApp::cbLongPress()
{
#ifdef SERIALDEBUG
    Serial.println("LONG PRESS");
#endif
    if (screenPages->GetPage() == 4)
    {
        auto permut4 = mmPumpSwaps->Get("4pmp");
        permut4->ResetAllRuntimes();
        permut4->ResetAllStartCounts();
    }

    if (screenPages->GetPage() == 5)
    {
        auto float1 = this->mmVariables->Get<float>("varFloat1");
        float1->SetValue(50.0f);
    }
}

MyApp::MyApp(ExecutionEnv *executionEnv, ModbusClientRTU &MBRTU, MadMax::ModbusServerMemoryManager *modbusServerManager, DigitalInputs *digitalInputs, DigitalOutputs *digitalOutputs)
{
    this->executionEnv = executionEnv;
    this->digitalInputs = digitalInputs;
    this->digitalOutputs = digitalOutputs;
    this->modbusServerManager = modbusServerManager;
    this->scheduler = new ModbusScheduler(MBRTU);
    this->screenPages = new mmScreenPages(executionEnv, 6, [this]()
                                          { this->cbShortPress(); }, [this]()
                                          { this->cbLongPress(); });

    lastMilliDisplay = 0;

    mmPIDs = new MadMax::PIDProvider(executionEnv);
    mmMinOnOffs = new MadMax::MinOnOffProvider(executionEnv);
    mmDelayOnOffs = new MadMax::DelayOnOffProvider(executionEnv);
    mmRunTimes = new MadMax::RunTimeProvider(executionEnv);
    mmTPulses = new MadMax::TPulseProvider(executionEnv);
    mmLinears = new MadMax::LinearProvider(executionEnv);
    mmFeedbackErrors = new MadMax::FeedbackErrorProvider(executionEnv);
    mmDigitalEquipments = new MadMax::DigitalEquipmentProvider(executionEnv);
    mmPumpSwaps = new MadMax::PumpSwapProvider(executionEnv);
    mmVariables = new MadMax::VariableProvider(executionEnv);
    mmAccums = new MadMax::AccumProvider(executionEnv);
}

MyApp::~MyApp()
{
    delete this->scheduler;
    // delete feedbackError;
}

void MyApp::Init()
{
    // Initialize application-specific settings
    /*
    mmPIDs.create("pid1", executionEnv);
    mmMinOnOffs.create("minOnOff1", executionEnv, false);
    mmMinOnOffs.create("minOnOff2", executionEnv, false);
    mmMinOnOffs.create("minOnOff3", executionEnv, false);
    mmDelayOnOffs.create("delayOnOff1", executionEnv, false);
    mmRunTimes.create("runTime1", executionEnv, 0);
    mmTPulses.create("tPulse1", executionEnv, false);
    mmLinears.create("linear1", executionEnv);
    mmDigitalEquipments.create("de0", executionEnv);
    */

    // Au démarrage de l'application, on crée les objets nécessaires et on charge leurs valeurs depuis la base de données de persistance et on les associe à des adresses de registres Modbus

    mmVariables->Create<float>("varFloat1", 4 << 16 | 32, {.value = 0.0f});
    mmVariables->Create<float>("AB", 4 << 16 | 34, {.value = 10.0f});
    mmVariables->Create<float>("XW", 4 << 16 | 36, {.value = 12.0f});
    mmVariables->Create<int8_t>("myChar", 4 << 16 | 38, {.value = 0});
    mmVariables->Create<bool>("myBool", 4 << 16 | 39, {.value = true});
    mmVariables->Create<uint8_t>("pumps_nbr", 4 << 16 | 50, {.value = 3});

    mmPumpSwaps->Create("4pmp", 4, 4, 10000);

    mmAccums->Create<uint64_t>("accum1", 0, {.value = 0});

    mmDigitalEquipments->Create("de0", 4 << 16 | 40, {.runTime = 10, .startCount = 1});

    mmTPulses->Create("tp0", 0, false);

    mmFeedbackErrors->Create("fe0", 0);

    mmDelayOnOffs->Create("delay0", 0, false);

    mmRunTimes->Create("rt0", 0, {.value = 100});

    /*
        // Tache modbus 1: lecture de 2 registres à l'adresse 0 du slave 1 toutes les secondes
        t1.Set(1, MBFunction::ReadHoldingRegisters, 0, 2, 1000, nullptr, [](const MBTask &r)
               { if (r.state == MBTaskState::Done) { for (auto v : r.readData) Serial.println(v);} });
        this->scheduler->addTask(t1);

        // Tache modbus 2: écriture d'un registre à l'adresse 0 du slave 1 toutes les secondes
        t2.Set(1, MBFunction::WriteSingleRegister, 0, 1, 100, &writeValue, [](const MBTask &r) {});
        this->scheduler->addTask(t2);

        // Tache modbus 3: lecture de 1 registre à l'adresse 2 du slave 1 toutes les secondes
        t3.Set(1, MBFunction::ReadHoldingRegisters, 2, 1, 1000, nullptr, [](const MBTask &r)
               { if (r.state == MBTaskState::Done) { for (auto v : r.readData) Serial.println(v);} });
        this->scheduler->addTask(t3);

        // Tache modbus 4: lecture de 1 registre à l'adresse 3 du slave 1 toutes les secondes
        t4.Set(1, MBFunction::ReadHoldingRegisters, 3, 1, 1000, nullptr, [](const MBTask &r)
               { if (r.state == MBTaskState::Done) { for (auto v : r.readData) Serial.println(v); } });
        this->scheduler->addTask(t4);

        // Tache modbus 5: lecture de 1 registre à l'adresse 4 du slave 1 toutes les secondes
        t5.Set(1, MBFunction::ReadHoldingRegisters, 4, 1, 1000, nullptr, [](const MBTask &r)
               { if (r.state == MBTaskState::Done) { for (auto v : r.readData) Serial.println(v); } });
        this->scheduler->addTask(t5);

        // Tache modbus
        t6.Set(2, MBFunction::ReadHoldingRegisters, 22, 2, 1000, nullptr, [](const MBTask &r)
               { if (r.state == MBTaskState::Done) { counter1 = *(uint32_t*)&r.readData[0]; } });
        this->scheduler->addTask(t6);

        // Tache modbus
        t7.Set(2, MBFunction::ReadInputRegisters, 15, 1, 100, nullptr, [](const MBTask &r)
               { if (r.state == MBTaskState::Done) { for (auto v : r.readData) Serial.println(v); } });
        this->scheduler->addTask(t7);*/

    // Tache modbus
    /*
    t8.Set(3, MBFunction::ReadHoldingRegisters, 0, 1, 100, nullptr, [](const MBTask &r)
           { if (r.state == MBTaskState::Done) { for (auto v : r.readData) Serial.println(v); } });
    this->scheduler->addTask(t8);

    uint16_t *writeValues = new uint16_t[4]{1, 2, 3, 4};

    t9.Set(3, MBFunction::WriteMultipleRegisters, 1, 4, 1000, writeValues, [](const MBTask &r)
           { if (r.state == MBTaskState::Done) { for (auto v : r.readData) Serial.println(v); } });
    this->scheduler->addTask(t9);

    t10.Set2(3, MBFunction::WriteMultipleCoils, 2, writeCoils.coils(), 100, writeCoils.data(), [](const MBTask &r)
             { if (r.state == MBTaskState::Done) { for (auto v : r.readData) Serial.println(v); } });
    this->scheduler->addTask(t10);

    floatValue = (float *)new uint16_t[2]; // reserve 2 registers for a float value
    *floatValue = 0;

    t11.Set(3, MBFunction::WriteMultipleRegisters, 5, 2, 1000, (uint16_t *)floatValue, [](const MBTask &r)
            { if (r.state == MBTaskState::Done) { for (auto v : r.readData) Serial.println(v); } });
    this->scheduler->addTask(t11);
    */
}

MadMax::DigitalEquipmentStatus status;
uint8_t memMinute;

void MyApp::Loop()
{
    myTick = executionEnv->Execute();

    // Clear display
    executionEnv->Display()->clearDisplay();

    screenPages->Evaluate(digitalRead(GPIO0) == LOW);

    /*
    mmDigitalEquipment *de0 = mmDigitalEquipments.get("de0");
    de0->Evaluate(true, true, false, false, 10000, &status);

    uint64_t *rt = this->modbusServerManager->AssociateHoldingRegister<uint64_t>(12);
    uint64_t *sc = this->modbusServerManager->AssociateHoldingRegister<uint64_t>(16);

    *rt = status.runTimeValue;
    *sc = status.startCountValue;
    */

    uint8_t minute = executionEnv->getMinute();

    mmVariables->RefreshFromModbusRegisters();

    auto permut4 = mmPumpSwaps->Get("4pmp");

    MadMax::PumpSwapResult result = permut4->Evaluate(
        [this](const std::vector<MadMax::SetPumpValue *> &pumps)
        {
            pumps[0]->Fault = this->digitalInputs->Get(8);
            pumps[0]->Feedback = this->digitalInputs->Get(9);
            pumps[1]->Fault = this->digitalInputs->Get(10);
            pumps[1]->Feedback = this->digitalInputs->Get(11);
            pumps[2]->Fault = this->digitalInputs->Get(12);
            pumps[2]->Feedback = this->digitalInputs->Get(13);
            pumps[3]->Fault = this->digitalInputs->Get(14);
            pumps[3]->Feedback = this->digitalInputs->Get(15);
        },
        {(this->mmVariables->Get<uint8_t>("pumps_nbr")->GetValue() > 0),
         (this->mmVariables->Get<uint8_t>("pumps_nbr")->GetValue() > 1),
         (this->mmVariables->Get<uint8_t>("pumps_nbr")->GetValue() > 2),
         (this->mmVariables->Get<uint8_t>("pumps_nbr")->GetValue() > 3)},
        false, this->digitalInputs->Get(0));

    digitalOutputs->Set(0, result.PumpsStatus[0]->output);
    digitalOutputs->Set(1, result.PumpsStatus[1]->output);
    digitalOutputs->Set(2, result.PumpsStatus[2]->output);
    digitalOutputs->Set(3, result.PumpsStatus[3]->output);
    // digitalOutputs->Set(4, result.PumpsStatus[0]->faults.fault);
    // digitalOutputs->Set(5, result.PumpsStatus[1]->faults.fault);
    // digitalOutputs->Set(6, result.PumpsStatus[2]->faults.fault);
    // digitalOutputs->Set(7, result.PumpsStatus[3]->faults.fault);

    uint16_t *AvailablePumps = this->modbusServerManager->AssociateHoldingRegister<uint16_t>(12);
    uint16_t *RequestedPumps = this->modbusServerManager->AssociateHoldingRegister<uint16_t>(13);
    uint16_t *RunningPumps = this->modbusServerManager->AssociateHoldingRegister<uint16_t>(14);
    uint16_t *CapacityState = this->modbusServerManager->AssociateHoldingRegister<uint16_t>(15);
    uint64_t *Rt1 = this->modbusServerManager->AssociateHoldingRegister<uint64_t>(16);
    uint64_t *Rt2 = this->modbusServerManager->AssociateHoldingRegister<uint64_t>(20);
    uint64_t *Rt3 = this->modbusServerManager->AssociateHoldingRegister<uint64_t>(24);
    uint64_t *Rt4 = this->modbusServerManager->AssociateHoldingRegister<uint64_t>(28);

    *AvailablePumps = result.AvailablePumps;
    *RequestedPumps = result.RequestedPumps;
    *RunningPumps = result.RunningPumps;
    *CapacityState = (uint16_t)result.CapacityState;

    *Rt1 = result.PumpsStatus[0]->runTimeValue;
    *Rt2 = result.PumpsStatus[1]->runTimeValue;
    *Rt3 = result.PumpsStatus[2]->runTimeValue;
    *Rt4 = result.PumpsStatus[3]->runTimeValue;

    auto fe0 = mmFeedbackErrors->Get("fe0")->Evaluate(false, false, 5000, 6000, false, MadMax::FeedbackErrorOption::Both);

    auto delay0 = mmDelayOnOffs->Get("delay0")->Evaluate(true, 60000, 0);

    mmRunTimes->Get("rt0")->Evaluate(true);

    /*// on créer un alias vers le registre 32 et on écrit sa valeur sur la variable persistante Float1
    float *Float1 = this->modbusServerManager->AssociateHoldingRegister<float>(32);
    mmVariables->SetValue("varFloat1", *Float1);

    float *AB = this->modbusServerManager->AssociateHoldingRegister<float>(34);
    mmVariables->SetValue("AB", *AB);

    float *XW = this->modbusServerManager->AssociateHoldingRegister<float>(36);
    mmVariables->SetValue("XW", *XW);

    uint8_t *myChar = this->modbusServerManager->AssociateHoldingRegister<uint8_t>(38);
    mmVariables->SetValue("myChar", *myChar);

    bool *myBool = this->modbusServerManager->AssociateHoldingRegister<bool>(39);
    mmVariables->SetValue("myBool", *myBool);*/

    auto accum1 = mmAccums->Get<uint64_t>("accum1");
    uint64_t *Accum1 = this->modbusServerManager->AssociateHoldingRegister<uint64_t>(40);
    *Accum1 = accum1->Evaluate(memMinute != minute, 1, false);

    if (memMinute != minute)
    {
        // mmDigitalEquipments.SavePersistencyValues("de0", executionEnv);
        // Serial.println(F("Save 'de0' persistancy values"));

        mmPumpSwaps->SavePersistencyValuesToMem("4pmp");
        Serial.println(F("Saved '4pmp' persistancy values to memory"));
        mmAccums->SavePersistencyValuesToMem("accum1");
        Serial.println(F("Saved 'accum1' persistancy values to memory"));

#ifdef SERIALDEBUG
        Serial.println(F("Saved persistancy values successfully"));
#endif
    }
    memMinute = minute;

    executionEnv->GetMiniPrefs()->defragStep();

    uint64_t delta = Millis64::millis64() - lastMilliDisplay;

    if (delta > DISPLAYPERIOD)
    {
        lastMilliDisplay = Millis64::millis64();

        switch (screenPages->GetPage())
        {
        case 0:
        {
            // Display useful information on OLED screen /////////////////////////////////////////////////
            // executionEnv->Display()->setFont(FreeSansBold9pt7bBitmaps);
            executionEnv->Display()->fillRect(0, 0, 128, 10, SH110X_WHITE);
            executionEnv->Display()->setCursor(0, 1);
            executionEnv->Display()->setTextColor(SH110X_BLACK);
            sprintf(message, ">>>  SYSTEM PAGE  <<<");
            executionEnv->Display()->println(message);
            executionEnv->Display()->setTextColor(SH110X_WHITE);

            // Display useful information on OLED screen /////////////////////////////////////////////////
            executionEnv->Display()->setCursor(0, 20);
            sprintf(message, "Ticks: %ld", myTick);
            executionEnv->Display()->println(message);

            // Display useful information on OLED screen /////////////////////////////////////////////////
            executionEnv->Display()->setCursor(0, 30);
            sprintf(message, "Last op. time: %ld ms", executionEnv->StopMetrics());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 40);
            sprintf(message, "%04d/%02d/%02d - %02d:%02d:%02d", executionEnv->getYear(), executionEnv->getMonth(), executionEnv->getDay(), executionEnv->getHour(), executionEnv->getMinute(), executionEnv->getSecond());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 50);
            sprintf(message, "RAM usage: %0.2f%%", executionEnv->GetRamUsage());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 60);
            sprintf(message, "FRAM usage: %0.2f%%", executionEnv->GetMiniPrefs()->GetFRAMUsage());
            executionEnv->Display()->println(message);
        }
        break;

        case 1:
        {
            char ipBuffer[16];
            // Display useful information on OLED screen /////////////////////////////////////////////////
            executionEnv->Display()->fillRect(0, 0, 128, 10, SH110X_WHITE);
            executionEnv->Display()->setCursor(0, 1);
            executionEnv->Display()->setTextColor(SH110X_BLACK);
            sprintf(message, ">>> ETHERNET PAGE <<<");
            executionEnv->Display()->println(message);
            executionEnv->Display()->setTextColor(SH110X_WHITE);

            auto eth = executionEnv->GetActualETH();

            // Display useful information on OLED screen /////////////////////////////////////////////////
            executionEnv->Display()->setCursor(0, 20);
            sprintf(message, "DHCP: %s", executionEnv->GetETHProperties().Dhcp ? "Active" : "Inactive");
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 30);
            sprintf(message, "IP:   %s", eth.Ip.toString().c_str());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 40);
            sprintf(message, "Mask: %s", eth.Netmask.toString().c_str());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 50);
            sprintf(message, "Gatw: %s", eth.Gateway.toString().c_str());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 60);
            sprintf(message, "DNS1: %s", eth.Dns1.toString().c_str());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 70);
            sprintf(message, "DNS2: %s", eth.Dns2.toString().c_str());
            executionEnv->Display()->println(message);
        }
        break;

        case 2:
        {
            char ipBuffer[16];
            // Display useful information on OLED screen /////////////////////////////////////////////////
            executionEnv->Display()->fillRect(0, 0, 128, 10, SH110X_WHITE);
            executionEnv->Display()->setCursor(0, 1);
            executionEnv->Display()->setTextColor(SH110X_BLACK);
            sprintf(message, "> WIFI STATION PAGE <");
            executionEnv->Display()->println(message);
            executionEnv->Display()->setTextColor(SH110X_WHITE);

            auto sta = executionEnv->GetActualSTA();
            auto staProps = executionEnv->GetSTAProperties();

            // Display useful information on OLED screen /////////////////////////////////////////////////
            executionEnv->Display()->setCursor(0, 20);
            sprintf(message, "DHCP: %s", executionEnv->GetSTAProperties().Dhcp ? "Active" : "Inactive");
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 30);
            sprintf(message, "IP:   %s", sta.Ip.toString().c_str());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 40);
            sprintf(message, "Mask: %s", sta.Netmask.toString().c_str());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 50);
            sprintf(message, "Gatw: %s", sta.Gateway.toString().c_str());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 60);
            sprintf(message, "DNS1: %s", sta.Dns1.toString().c_str());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 70);
            sprintf(message, "DNS2: %s", sta.Dns2.toString().c_str());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 80);
            sprintf(message, "SSID: %s", staProps.SSID.c_str());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 90);
            sprintf(message, "Pwd.: %s", staProps.Password.c_str());
            executionEnv->Display()->println(message);
        }
        break;

        case 3:
        {
            char ipBuffer[16];
            // Display useful information on OLED screen /////////////////////////////////////////////////
            executionEnv->Display()->fillRect(0, 0, 128, 10, SH110X_WHITE);
            executionEnv->Display()->setCursor(0, 1);
            executionEnv->Display()->setTextColor(SH110X_BLACK);
            sprintf(message, ">>>  WIFI AP PAGE <<<");
            executionEnv->Display()->println(message);
            executionEnv->Display()->setTextColor(SH110X_WHITE);

            auto wap = executionEnv->GetWAPProperties();

            // Display useful information on OLED screen /////////////////////////////////////////////////
            executionEnv->Display()->setCursor(0, 20);
            sprintf(message, "IP:   %s", wap.Ip.toString().c_str());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 30);
            sprintf(message, "Mask: %s", wap.Netmask.toString().c_str());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 40);
            sprintf(message, "Gatw: %s", wap.Gateway.toString().c_str());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 50);
            sprintf(message, "SSID: %s", wap.SSID.c_str());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 60);
            sprintf(message, "Pwd.: %s", wap.Password.c_str());
            executionEnv->Display()->println(message);
        }
        break;

        case 4:
        {
            // Display useful information on OLED screen /////////////////////////////////////////////////
            executionEnv->Display()->fillRect(0, 0, 128, 10, SH110X_WHITE);
            executionEnv->Display()->setCursor(0, 1);
            executionEnv->Display()->setTextColor(SH110X_BLACK);
            sprintf(message, ">>  SYSTEM 4 PUMPS <<");
            executionEnv->Display()->println(message);
            executionEnv->Display()->setTextColor(SH110X_WHITE);

            for (int i = 0; i < result.TotalPumps; i++)
            {
                if (result.PumpsStatus[i]->output)
                    executionEnv->Display()->fillCircleHelper(30 + 20 * i, 20, 5, 15, 0, SH110X_WHITE);
                else
                    executionEnv->Display()->drawCircleHelper(30 + 20 * i, 20, 5, 15, SH110X_WHITE);
            }

            // Display useful information on OLED screen /////////////////////////////////////////////////
            executionEnv->Display()->setCursor(0, 30);
            sprintf(message, "Rt. pump 1: %d s", result.PumpsStatus[0]->runTimeValue / 1000);
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 40);
            sprintf(message, "Rt. pump 2: %d s", result.PumpsStatus[1]->runTimeValue / 1000);
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 50);
            sprintf(message, "Rt. pump 3: %d s", result.PumpsStatus[2]->runTimeValue / 1000);
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 60);
            sprintf(message, "Rt. pump 4: %d s", result.PumpsStatus[3]->runTimeValue / 1000);
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 70);
            sprintf(message, "Start pump 1: %d", result.PumpsStatus[0]->startCountValue);
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 80);
            sprintf(message, "Start pump 2: %d", result.PumpsStatus[1]->startCountValue);
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 90);
            sprintf(message, "Start pump 3: %d", result.PumpsStatus[2]->startCountValue);
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 100);
            sprintf(message, "Start pump 4: %d", result.PumpsStatus[3]->startCountValue);
            executionEnv->Display()->println(message);
        }
        break;

        case 5:
        {
            // Display useful information on OLED screen /////////////////////////////////////////////////
            executionEnv->Display()->fillRect(0, 0, 128, 10, SH110X_WHITE);
            executionEnv->Display()->setCursor(0, 1);
            executionEnv->Display()->setTextColor(SH110X_BLACK);
            sprintf(message, ">>>   VARIABLES   <<<");
            executionEnv->Display()->println(message);
            executionEnv->Display()->setTextColor(SH110X_WHITE);

            executionEnv->Display()->setCursor(0, 30);
            sprintf(message, "varFloat1: %.2f", mmVariables->Get<float>("varFloat1")->GetValue());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 40);
            sprintf(message, "AB: %.2f", mmVariables->Get<float>("AB")->GetValue());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 50);
            sprintf(message, "XW: %.2f", mmVariables->Get<float>("XW")->GetValue());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 60);
            sprintf(message, "myChar: %d", mmVariables->Get<int8_t>("myChar")->GetValue());
            executionEnv->Display()->println(message);

            executionEnv->Display()->setCursor(0, 70);
            sprintf(message, "myBool: %s", mmVariables->Get<bool>("myBool")->GetValue() ? "true" : "false");
            executionEnv->Display()->println(message);
        }
        break;
        }

        executionEnv->Display()->display();
    }

    /*
    bool cmd = digitalInputs->Get(0);
    bool feedback = digitalInputs->Get(1);
    bool ack = digitalInputs->Get(2);



    bool newCmd = digitalEquipment->Evaluate(cmd, feedback, ack, ack, 10000, &status);
    digitalOutputs->Set(0, newCmd);

    uint64_t *rt = this->modbusServerManager->AssociateHoldingRegister<uint64_t>(12);
    uint64_t *sc = this->modbusServerManager->AssociateHoldingRegister<uint64_t>(16);

    *rt = status.runTimeValue;
    *sc = status.startCountValue;
    */

    /*

    writeCoils.set(0, blink);
    writeCoils.set(1, blink2);
    writeCoils.set(2, blink3);
    writeCoils.set(3, blink4);

    this->scheduler->loop();

    mmPID *pid1 = mmPIDs.get("pid1");
    mmMinOnOff *minOnOff = mmMinOnOffs.get("minOnOff1");
    mmMinOnOff *minOnOff2 = mmMinOnOffs.get("minOnOff2");
    mmMinOnOff *minOnOff3 = mmMinOnOffs.get("minOnOff3");

    float *linearInput = this->modbusServerManager->AssociateHoldingRegister<float>(10);
    float *linearValue = this->modbusServerManager->AssociateHoldingRegister<float>(12);
    const PointXY table[10] = {{0.0f, 0.0f}, {10.0f, 100.0f}, {20.0f, 50.0f}, {30.0f, 150.0f}, {40.0f, 120.0f}, {50.0f, 200.0f}, {60.0f, 180.0f}, {70.0f, 250.0f}, {80.0f, 220.0f}, {90.0f, 300.0f}};
    *linearValue = mmLinears.get("linear1")->Evaluate(table, 10, *linearInput, true);

    DelayStatus status;

    blink = mmDelayOnOffs.get("delayOnOff1")->Evaluate(!blink, 500, 500, &status);
    blink2 = minOnOff->Evaluate(!blink2, 5000, 5000);
    blink3 = minOnOff2->Evaluate(!blink3, 7500, 7500);
    blink4 = minOnOff3->Evaluate(!blink4, 9000, 9000);

    int16_t *remtime = this->modbusServerManager->AssociateHoldingRegister<int16_t>(14);
    *remtime = (int16_t)status.remainingTime;

    if (digitalInputs->Get(0))
    {
        mmDelayOnOffs.get("delayOnOff1")->EmergencyOff();
        minOnOff->EmergencyOff();
        measure = 18.0;
    }

    if (digitalInputs->Get(1))
    {
        mmDelayOnOffs.get("delayOnOff1")->EmergencyOn();
        minOnOff->EmergencyOn();
        measure = 24.0;
    }

    digitalOutputs->Set(11, feedbackError->Evaluate(digitalInputs->Get(0), digitalInputs->Get(1), 5000, 5000));

    if (digitalInputs->Get(7))
    {
        throw std::runtime_error("PANIC");
    }

    digitalOutputs->Set(8, blink);
    digitalOutputs->Set(9, blink2);

    mmRunTime *runTime1 = mmRunTimes.get("runTime1");
    runTime1->Evaluate(blink2);

    pidValue = pid1->Evaluate(1, 20.0, measure, 3.0, 30.0, 0.1, 0.0, 100.0, 0.0);

    digitalOutputs->Set(10, mmTPulses.get("tPulse1")->Evaluate(blink2, 1000));

    writeValue = (uint16_t)(blink) | ((uint16_t)blink2) << 1;

    *floatValue = myTick;
    */
}