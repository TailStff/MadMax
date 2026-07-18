#pragma once

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "myApp.h"
#include "ExecutionEnv.h"

class WebAPI
{
private:
    AsyncWebServer &server;
    MyApp *myApp;
    ExecutionEnv *executionEnv;

    // -------- Routing --------
    void apiRouter(AsyncWebServerRequest *request);
    void apiBodyHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);

    void handleAPI(AsyncWebServerRequest *r);

    // -------- Variables --------
    void getVariablesList(AsyncWebServerRequest *request);
    void getVariableDetail(const std::string &name, AsyncWebServerRequest *request);
    void getVariableProperty(const std::string &name, const std::string &property, AsyncWebServerRequest *request);

    void getAccumsList(AsyncWebServerRequest *request);
    void getAccumDetail(const std::string &name, AsyncWebServerRequest *request);

    void getDigitalEquipmentsList(AsyncWebServerRequest *request);
    void getDigitalEquipmentDetail(const std::string &name, AsyncWebServerRequest *request);

    void getPumpSwapsList(AsyncWebServerRequest *request);
    void getPumpSwapDetail(const std::string &name, AsyncWebServerRequest *request);
    void triggerPumpSwapAction(const std::string &name, const std::string &action, AsyncWebServerRequest *request);

    void getTPulsesList(AsyncWebServerRequest *request);
    void getTPulseDetail(const std::string &name, AsyncWebServerRequest *request);

    void getFeedbackErrorsList(AsyncWebServerRequest *request);
    void getFeedbackErrorDetail(const std::string &name, AsyncWebServerRequest *request);

    void getDelayOnOffsList(AsyncWebServerRequest *request);
    void getDelayOnOffDetail(const std::string &name, AsyncWebServerRequest *request);

    void getRunTimesList(AsyncWebServerRequest *request);
    void getRunTimeDetail(const std::string &name, AsyncWebServerRequest *request);

    // -------- Interfaces --------
    void getInterfacesList(AsyncWebServerRequest *request);
    void getInterfaceEthProperties(JsonObject obj);
    void getInterfaceStaProperties(JsonObject obj);
    void getInterfaceWapProperties(JsonObject obj);

    void getInterfaceEth(AsyncWebServerRequest *request);
    void getInterfaceSta(AsyncWebServerRequest *request);
    void getInterfaceWap(AsyncWebServerRequest *request);

    void setInterfaceEth(JsonDocument doc, AsyncWebServerRequest *request);
    void setInterfaceSta(JsonDocument doc, AsyncWebServerRequest *request);
    void setInterfaceWap(JsonDocument doc, AsyncWebServerRequest *request);

public:
    WebAPI(AsyncWebServer &server, MyApp *app, ExecutionEnv *env);
    void Setup();
};