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

    // -------- Pages --------
    void homePage(AsyncWebServerRequest *request);
    void styles(AsyncWebServerRequest *request);

    // -------- Routing --------
    void apiRouter(AsyncWebServerRequest *request);
    void apiBodyHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);

    // -------- Variables --------
    void getVariablesList(AsyncWebServerRequest *request);
    void getVariableDetail(const String &name, AsyncWebServerRequest *request);
    void getVariableProperty(const std::string &name, const std::string &property, AsyncWebServerRequest *request);

    void getAccumsList(AsyncWebServerRequest *request);
    void getAccumDetail(const String &name, AsyncWebServerRequest *request);

    void getDigitalEquipmentsList(AsyncWebServerRequest *request);
    void getDigitalEquipmentDetail(const String &name, AsyncWebServerRequest *request);

    void getPumpSwapsList(AsyncWebServerRequest *request);
    void getPumpSwapDetail(const String &name, AsyncWebServerRequest *request);

    void getTPulsesList(AsyncWebServerRequest *request);
    void getTPulseDetail(const String &name, AsyncWebServerRequest *request);

    void getFeedbackErrorsList(AsyncWebServerRequest *request);
    void getFeedbackErrorDetail(const String &name, AsyncWebServerRequest *request);

    void getDelayOnOffsList(AsyncWebServerRequest *request);
    void getDelayOnOffDetail(const String &name, AsyncWebServerRequest *request);

    // -------- Interfaces --------
    void getInterfaceEth(AsyncWebServerRequest *request);
    void getInterfaceSta(AsyncWebServerRequest *request);
    void getInterfaceWap(AsyncWebServerRequest *request);
    void setInterfaceEth(const char *jsonBuffer, AsyncWebServerRequest *request);
    void setInterfaceSta(const char *jsonBuffer, AsyncWebServerRequest *request);

public:
    WebAPI(AsyncWebServer &server, MyApp *app, ExecutionEnv *env);
    void Setup();
};