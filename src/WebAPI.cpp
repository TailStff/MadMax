#include "WebAPI.h"
#include <LittleFS.h>

#include <ArduinoJson.h>
#include "VariableValue.h"

WebAPI::WebAPI(AsyncWebServer &server, MyApp *app, ExecutionEnv *env) : server(server), myApp(app), executionEnv(env) {}

// -------- Pages --------
/*
void WebAPI::jquery(AsyncWebServerRequest *request)
{
    request->send(LittleFS, "/jquery-4.0.0.min.js", "application/javascript");
}

void WebAPI::jqueryUI(AsyncWebServerRequest *request)
{
    request->send(LittleFS, "/jquery-ui.min.js", "application/javascript");
}

void WebAPI::jqueryUICSS(AsyncWebServerRequest *request)
{
    request->send(LittleFS, "/jquery-ui.min.css", "text/css");
}

void WebAPI::homePage(AsyncWebServerRequest *request)
{
    request->send(LittleFS, "/index.html", "text/html");
}

void WebAPI::styles(AsyncWebServerRequest *request)
{
    request->send(LittleFS, "/styles.css", "text/css");
}

void WebAPI::application(AsyncWebServerRequest *request)
{
    request->send(LittleFS, "/app.js", "application/javascript");
}

void WebAPI::variablesNavigator(AsyncWebServerRequest *request)
{
    request->send(LittleFS, "/variablesNavigator.js", "application/javascript");
}

void WebAPI::variablesNavigatorCSS(AsyncWebServerRequest *request)
{
    request->send(LittleFS, "/variablesNavigator.css", "text/css");
}*/

/// Setup the web server routes and handlers
void WebAPI::Setup()
{
    // Files
    /*server.on("/", HTTP_GET, [this](AsyncWebServerRequest *r)
              { homePage(r); });*/

    /*
server.on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *r)
    { homePage(r); });

server.on("/jquery-4.0.0.min.js", HTTP_GET, [this](AsyncWebServerRequest *r)
    { jquery(r); });

server.on("/jquery-ui.min.js", HTTP_GET, [this](AsyncWebServerRequest *r)
    { jqueryUI(r); });

server.on("/jquery-ui.min.css", HTTP_GET, [this](AsyncWebServerRequest *r)
    { jqueryUICSS(r); });

server.on("/styles.css", HTTP_GET, [this](AsyncWebServerRequest *r)
    { styles(r); });

server.on("/variablesNavigator.css", HTTP_GET, [this](AsyncWebServerRequest *r)
    { variablesNavigatorCSS(r); });

server.on("/app.js", HTTP_GET, [this](AsyncWebServerRequest *r)
    { application(r); });

server.on("/variablesNavigator.js", HTTP_GET, [this](AsyncWebServerRequest *r)
    { variablesNavigator(r); });*/

    // API
    server.on("/API/*", HTTP_GET, [this](AsyncWebServerRequest *r)
              { handleAPI(r); });

    server.on("/API/*", HTTP_POST, [this](AsyncWebServerRequest *r)
              { handleAPI(r); }, NULL, [this](AsyncWebServerRequest *r, uint8_t *data, size_t len, size_t index, size_t total)
              { apiBodyHandler(r, data, len, index, total); });

    /*
            server.on("/API", HTTP_GET, [this](AsyncWebServerRequest *r)
                      { apiRouter(r); });

            server.on("/API", HTTP_POST, [this](AsyncWebServerRequest *r)
                      { apiRouter(r); }, NULL, [this](AsyncWebServerRequest *r, uint8_t *data, size_t len, size_t index, size_t total)
                      { apiBodyHandler(r, data, len, index, total); });*/

    // server.on("/API/interfaces", HTTP_GET, apiGetInterfaces);

    // server.on("/API/interfaces/:int", HTTP_GET, apiGetInterface);

    /*
    server.on("/INFOS", HTTP_GET, SendInfos);
    server.on("/DATAS", HTTP_GET, SendDatas);
    server.on("/RESET", HTTP_GET, ESPReset);
    server.on("/IP", HTTP_POST, setIPAddressResponse, NULL, setIPAddressExecute);
    server.on("/SETTINGS", HTTP_POST, setSettingsResponse, NULL, setSettingsExecute);
    server.on("/DATETIME", HTTP_POST, setDateTimeResponse, NULL, setDateTimeExecute);*/

    // server.onNotFound(notFound);

    server.serveStatic("/", LittleFS, "/")
        .setDefaultFile("index.html");

    server.begin();
}

void WebAPI::handleAPI(AsyncWebServerRequest *request)
{
    String path = request->url(); // ex: /API/Variables/test/value

    path.remove(0, 5); // enlève "/API/"

    // Split simple
    std::vector<String> parts;
    int start = 0;
    int idx;

    while ((idx = path.indexOf('/', start)) != -1)
    {
        parts.push_back(path.substring(start, idx));
        start = idx + 1;
    }
    parts.push_back(path.substring(start));

    if (request->method() == HTTP_GET)
    {

        // Dispatch
        if (parts.size() == 1)
        {
            if (parts[0] == "Variables")
                return getVariablesList(request);
            else if (parts[0] == "Accums")
                return getAccumsList(request);
            else if (parts[0] == "DigitalEquipments")
                return getDigitalEquipmentsList(request);
            else if (parts[0] == "PumpSwaps")
                return getPumpSwapsList(request);
            else if (parts[0] == "TPulses")
                return getTPulsesList(request);
            else if (parts[0] == "FeedbackErrors")
                return getFeedbackErrorsList(request);
            else if (parts[0] == "DelayOnOffs")
                return getDelayOnOffsList(request);
            else if (parts[0] == "RunTimes")
                return getRunTimesList(request);
        }
        else if (parts.size() == 2)
        {
            if (parts[0] == "Variables")
                return getVariableDetail(parts[1].c_str(), request);
            else if (parts[0] == "Accums")
                return getAccumDetail(parts[1].c_str(), request);
            else if (parts[0] == "DigitalEquipments")
                return getDigitalEquipmentDetail(parts[1].c_str(), request);
            else if (parts[0] == "PumpSwaps")
                return getPumpSwapDetail(parts[1].c_str(), request);
            else if (parts[0] == "TPulses")
                return getTPulseDetail(parts[1].c_str(), request);
            else if (parts[0] == "FeedbackErrors")
                return getFeedbackErrorDetail(parts[1].c_str(), request);
            else if (parts[0] == "DelayOnOffs")
                return getDelayOnOffDetail(parts[1].c_str(), request);
            else if (parts[0] == "RunTimes")
                return getRunTimeDetail(parts[1].c_str(), request);
        }
        else if (parts.size() == 3)
        {
            if (parts[0] == "Variables")
                return getVariableProperty(parts[1].c_str(), parts[2].c_str(), request);
        }
    }

    if (request->method() == HTTP_POST)
    {
        String url = request->url();

        if (parts.size() == 2)
        {
            if (parts[0] == "Variables")
            {
                // We check Variable existence before reading the body, to avoid reading a potentially big body if the variable doesn't exist or is not writable
                MadMax::DTOBase dto;
                if (myApp->GetVariablesProvider()->GetDetailDTO(parts[1].c_str(), dto))
                    return;
            }

            if (parts[0] == "Accums")
            {
                // We check Accum existence before reading the body, to avoid reading a potentially big body if the variable doesn't exist or is not writable
                MadMax::DTOBase dto;
                if (myApp->GetAccumsProvider()->GetDetailDTO(parts[1].c_str(), dto))
                    return;
            }
        }

        /*
        // Get a specific interface
        if (url.startsWith("/API/Interfaces/"))
        {
            String guid = url.substring(16);

            if (guid == "eth")
            {
                r->send(200, "text/plain", "Settings eth interface…");
                return;
            }
            if (guid == "sta")
            {
                r->send(200, "text/plain", "Settings sta interface…");
                return;
            }
        }*/
    }

    request->send(404, "text/plain", "Not found");
}

void WriteJson(JsonObject obj, const MadMax::FieldValue &field)
{
    std::visit(
        [&](auto &&val)
        {
            using T = std::decay_t<decltype(val)>;

            if constexpr (std::is_same_v<T, std::vector<uint8_t>> || std::is_same_v<T, std::vector<bool>>)
            {
                JsonArray arr = obj.createNestedArray(field.key);
                for (auto v : val)
                    arr.add(v);
            }
            else
            {
                obj[field.key] = val;
            }
        },
        field.value);
}

void WriteJson(JsonObject obj, const MadMax::DTOBase &dto)
{
    // JsonObject child = obj.createNestedObject(dto.objectName);

    // Champs simples
    for (const auto &field : dto.fields)
        WriteJson(obj, field);

    if (!dto.actions.empty())
    {
        JsonArray arr = obj.createNestedArray("actions");

        for (const auto &action : dto.actions)
            arr.add(action.key);
    }

    // Objets enfants
    for (const auto &childDto : dto.children)
    {
        JsonObject child = obj[childDto.objectName].to<JsonObject>();
        WriteJson(child, childDto);
    }
}

// -------- Variables --------

void WebAPI::getVariablesList(AsyncWebServerRequest *request)
{
    auto variables = myApp->GetVariablesProvider()->GetDTOs();

    AsyncResponseStream *response = request->beginResponseStream("application/json");

    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    for (const auto &dto : variables)
    {
        JsonObject obj = arr.add<JsonObject>();
        obj["name"] = dto.objectName;

        for (auto &field : dto.fields)
        {
            WriteJson(obj, field);
        }
    }

    serializeJson(doc, *response);
    request->send(response);
}

void WebAPI::getVariableDetail(const std::string &name, AsyncWebServerRequest *request)
{
    MadMax::DTOBase dto;
    if (myApp->GetVariablesProvider()->GetDetailDTO(name.c_str(), dto))
    {
        JsonDocument doc;
        doc["name"] = dto.objectName;

        JsonObject obj = doc.as<JsonObject>();

        for (auto &field : dto.fields)
        {
            WriteJson(obj, field);
        }

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        request->send(response);
    }
    else
    {
        request->send(404, "text/plain", "Object not found");
    }
}

void WebAPI::getVariableProperty(const std::string &name, const std::string &property, AsyncWebServerRequest *request)
{
    MadMax::DTOBase dto;
    if (!myApp->GetVariablesProvider()->GetDetailDTO(name.c_str(), dto))
    {
        request->send(404, "text/plain", "Object not found");
        return;
    }

    JsonDocument doc;

    auto it = std::find_if(dto.fields.begin(), dto.fields.end(), [&](const MadMax::FieldValue &f)
                           { return f.key == property; });

    JsonObject obj = doc.to<JsonObject>();

    if (it != dto.fields.end())
    {
        WriteJson(obj, *it);
    }
    else
    {
        request->send(404, "text/plain", "Property not found");
        return;
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
}

// Accums
void WebAPI::getAccumsList(AsyncWebServerRequest *request)
{
    auto accums = myApp->GetAccumsProvider()->GetDTOs();

    AsyncResponseStream *response = request->beginResponseStream("application/json");

    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    for (const auto &dto : accums)
    {
        JsonObject obj = arr.add<JsonObject>();
        obj["name"] = dto.objectName;

        for (auto &field : dto.fields)
        {
            WriteJson(obj, field);
        }
    }

    serializeJson(doc, *response);
    request->send(response);
}

void WebAPI::getAccumDetail(const std::string &name, AsyncWebServerRequest *request)
{
    MadMax::DTOBase dto;
    if (myApp->GetAccumsProvider()->GetDetailDTO(name.c_str(), dto))
    {
        AsyncResponseStream *response = request->beginResponseStream("application/json");

        JsonDocument doc;
        doc["name"] = dto.objectName;

        JsonObject obj = doc.as<JsonObject>();

        for (auto &field : dto.fields)
        {
            WriteJson(obj, field);
        }

        serializeJson(doc, *response);
        request->send(response);
    }
    else
    {
        request->send(404, "text/plain", "Object not found");
    }
}

// Digital Equipments
void WebAPI::getDigitalEquipmentsList(AsyncWebServerRequest *request)
{
    auto de = myApp->GetDigitalEquipmentProvider()->GetDTOs();

    AsyncResponseStream *response = request->beginResponseStream("application/json");

    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    for (const auto &dto : de)
    {
        JsonObject obj = arr.add<JsonObject>();
        obj["name"] = dto.objectName;

        for (auto &field : dto.fields)
        {
            WriteJson(obj, field);
        }
    }

    serializeJson(doc, *response);
    request->send(response);
}

void WebAPI::getDigitalEquipmentDetail(const std::string &name, AsyncWebServerRequest *request)
{
    MadMax::DTOBase dto;
    if (myApp->GetDigitalEquipmentProvider()->GetDetailDTO(name.c_str(), dto))
    {
        JsonDocument doc;
        doc["name"] = dto.objectName;

        JsonObject obj = doc.as<JsonObject>();

        WriteJson(obj, dto);

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        request->send(response);
    }
    else
    {
        request->send(404, "text/plain", "Object not found");
    }
}

// Pump Swaps
void WebAPI::getPumpSwapsList(AsyncWebServerRequest *request)
{
    auto ps = myApp->GetPumpSwapProvider()->GetDTOs();

    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    /// We serialize the list of pump swaps, but not their details (to avoid potentially big response if there are many pump swaps), the details can be retrieved with a specific request for each pump swap
    for (const auto &dto : ps)
    {
        JsonObject obj = arr.add<JsonObject>();
        obj["name"] = dto.objectName;

        for (auto &field : dto.fields)
        {
            WriteJson(obj, field);
        }

        if (!dto.actions.empty())
        {
            JsonArray arr = obj.createNestedArray("actions");

            for (const auto &action : dto.actions)
                arr.add(action.key);
        }
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
}

void WebAPI::getPumpSwapDetail(const std::string &name, AsyncWebServerRequest *request)
{
    MadMax::DTOBase dto;
    if (myApp->GetPumpSwapProvider()->GetDetailDTO(name.c_str(), dto))
    {
        JsonDocument doc;
        doc["name"] = dto.objectName;

        JsonObject obj = doc.as<JsonObject>();

        WriteJson(obj, dto);

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        request->send(response);
    }
    else
    {
        request->send(404, "text/plain", "Object not found");
    }
}

// TPulses
void WebAPI::getTPulsesList(AsyncWebServerRequest *request)
{
    auto tp = myApp->GetTPulseProvider()->GetDTOs();

    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    for (const auto &dto : tp)
    {
        JsonObject obj = arr.add<JsonObject>();
        obj["name"] = dto.objectName;

        for (auto &field : dto.fields)
        {
            WriteJson(obj, field);
        }
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
}

void WebAPI::getTPulseDetail(const std::string &name, AsyncWebServerRequest *request)
{
    MadMax::DTOBase dto;
    if (myApp->GetTPulseProvider()->GetDetailDTO(name.c_str(), dto))
    {
        JsonDocument doc;
        doc["name"] = dto.objectName;

        JsonObject obj = doc.as<JsonObject>();

        WriteJson(obj, dto);

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        request->send(response);
    }
    else
    {
        request->send(404, "text/plain", "Object not found");
    }
}

// Feedback Errors
void WebAPI::getFeedbackErrorsList(AsyncWebServerRequest *request)
{
    auto tp = myApp->GetFeedbackErrorProvider()->GetDTOs();

    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    for (const auto &dto : tp)
    {
        JsonObject obj = arr.add<JsonObject>();
        obj["name"] = dto.objectName;

        for (auto &field : dto.fields)
        {
            WriteJson(obj, field);
        }
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
}

void WebAPI::getFeedbackErrorDetail(const std::string &name, AsyncWebServerRequest *request)
{
    MadMax::DTOBase dto;
    if (myApp->GetFeedbackErrorProvider()->GetDetailDTO(name.c_str(), dto))
    {
        JsonDocument doc;
        doc["name"] = dto.objectName;

        JsonObject obj = doc.as<JsonObject>();

        WriteJson(obj, dto);

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        request->send(response);
    }
    else
    {
        request->send(404, "text/plain", "Object not found");
    }
}

// DelayOnOffs
void WebAPI::getDelayOnOffsList(AsyncWebServerRequest *request)
{
    auto tp = myApp->GetDelayOnOffProvider()->GetDTOs();

    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    for (const auto &dto : tp)
    {
        JsonObject obj = arr.add<JsonObject>();
        obj["name"] = dto.objectName;

        for (auto &field : dto.fields)
        {
            WriteJson(obj, field);
        }
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
}

void WebAPI::getDelayOnOffDetail(const std::string &name, AsyncWebServerRequest *request)
{
    MadMax::DTOBase dto;
    if (myApp->GetDelayOnOffProvider()->GetDetailDTO(name.c_str(), dto))
    {
        JsonDocument doc;
        doc["name"] = dto.objectName;

        JsonObject obj = doc.as<JsonObject>();

        WriteJson(obj, dto);

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        request->send(response);
    }
    else
    {
        request->send(404, "text/plain", "Object not found");
    }
}

// RunTimes
void WebAPI::getRunTimesList(AsyncWebServerRequest *request)
{
    auto tp = myApp->GetRunTimeProvider()->GetDTOs();

    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    for (const auto &dto : tp)
    {
        JsonObject obj = arr.add<JsonObject>();
        obj["name"] = dto.objectName;

        for (auto &field : dto.fields)
        {
            WriteJson(obj, field);
        }
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
}

void WebAPI::getRunTimeDetail(const std::string &name, AsyncWebServerRequest *request)
{
    MadMax::DTOBase dto;
    if (myApp->GetRunTimeProvider()->GetDetailDTO(name.c_str(), dto))
    {
        JsonDocument doc;
        doc["name"] = dto.objectName;

        JsonObject obj = doc.as<JsonObject>();

        WriteJson(obj, dto);

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        request->send(response);
    }
    else
    {
        request->send(404, "text/plain", "Object not found");
    }
}

// -------- Interfaces --------

void WebAPI::getInterfaceEth(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("application/json");

    auto eth = executionEnv->GetETHProperties();
    JsonDocument doc;
    doc["dhcp"] = eth.Dhcp;
    doc["ip"] = eth.Ip.toString();
    doc["netmask"] = eth.Netmask.toString();
    doc["gateway"] = eth.Gateway.toString();
    doc["dns1"] = eth.Dns1.toString();
    doc["dns2"] = eth.Dns2.toString();

    serializeJson(doc, *response);
    request->send(response);
}

void WebAPI::getInterfaceSta(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("application/json");

    auto sta = executionEnv->GetSTAProperties();
    JsonDocument doc;
    doc["dhcp"] = sta.Dhcp;
    doc["ip"] = sta.Ip.toString();
    doc["netmask"] = sta.Netmask.toString();
    doc["gateway"] = sta.Gateway.toString();
    doc["dns1"] = sta.Dns1.toString();
    doc["dns2"] = sta.Dns2.toString();
    doc["ssid"] = sta.SSID.c_str();
    doc["password"] = sta.Password.c_str();

    serializeJson(doc, *response);
    request->send(response);
}

void WebAPI::getInterfaceWap(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("application/json");

    auto wap = executionEnv->GetWAPProperties();
    JsonDocument doc;
    doc["ip"] = wap.Ip.toString();
    doc["netmask"] = wap.Netmask.toString();
    doc["gateway"] = wap.Gateway.toString();
    doc["ssid"] = wap.SSID.c_str();
    doc["password"] = wap.Password.c_str();

    serializeJson(doc, *response);
    request->send(response);
}

void WebAPI::setInterfaceEth(const char *jsonBuffer, AsyncWebServerRequest *request)
{
    JsonDocument doc;
    if (deserializeJson(doc, jsonBuffer))
    {
        request->send(400, "text/plain", "Invalid JSON");
        return;
    }

    IPAddress ip, netmask, gateway, dns1, dns2;
    if (!ip.fromString(doc["ip"].as<const char *>()))
    {
        request->send(400, "text/plain", "Invalid IP");
        return;
    }
    if (!netmask.fromString(doc["netmask"].as<const char *>()))
    {
        request->send(400, "text/plain", "Invalid Netmask");
        return;
    }
    if (!gateway.fromString(doc["gateway"].as<const char *>()))
    {
        request->send(400, "text/plain", "Invalid Gateway");
        return;
    }
    if (!dns1.fromString(doc["dns1"].as<const char *>()))
    {
        request->send(400, "text/plain", "Invalid DNS1");
        return;
    }
    if (!dns2.fromString(doc["dns2"].as<const char *>()))
    {
        request->send(400, "text/plain", "Invalid DNS2");
        return;
    }

    IPConfigDhcp config;
    config.Dhcp = doc["dhcp"].as<bool>();
    config.Ip = ip;
    config.Netmask = netmask;
    config.Gateway = gateway;
    config.Dns1 = dns1;
    config.Dns2 = dns2;

    executionEnv->SetETHProperties(config);
    request->send(200, "text/plain", "OK");
}

void WebAPI::setInterfaceSta(const char *jsonBuffer, AsyncWebServerRequest *request)
{
    JsonDocument doc;
    if (deserializeJson(doc, jsonBuffer))
    {
        request->send(400, "text/plain", "Invalid JSON");
        return;
    }

    IPAddress ip, netmask, gateway, dns1, dns2;
    if (!ip.fromString(doc["ip"].as<const char *>()))
    {
        request->send(400, "text/plain", "Invalid IP");
        return;
    }
    if (!netmask.fromString(doc["netmask"].as<const char *>()))
    {
        request->send(400, "text/plain", "Invalid Netmask");
        return;
    }
    if (!gateway.fromString(doc["gateway"].as<const char *>()))
    {
        request->send(400, "text/plain", "Invalid Gateway");
        return;
    }
    if (!dns1.fromString(doc["dns1"].as<const char *>()))
    {
        request->send(400, "text/plain", "Invalid DNS1");
        return;
    }
    if (!dns2.fromString(doc["dns2"].as<const char *>()))
    {
        request->send(400, "text/plain", "Invalid DNS2");
        return;
    }

    IPConfigSTA config;
    config.Dhcp = doc["dhcp"].as<bool>();
    config.Ip = ip;
    config.Netmask = netmask;
    config.Gateway = gateway;
    config.Dns1 = dns1;
    config.Dns2 = dns2;
    config.SSID = doc["ssid"].as<String>();
    config.Password = doc["password"].as<String>();

    executionEnv->SetETHProperties(config);
    request->send(200, "text/plain", "OK");
}

// -------- Router --------
void WebAPI::apiRouter(AsyncWebServerRequest *request)
{
    String url = request->url();

    if (request->method() == HTTP_GET)
    {
        if (url.equals("/API/Interfaces"))
        {
            request->send(200, "text/plain", "eth, sta, wap, modbusRTU, modbusTCP");
            return;
        }

        if (url.startsWith("/API/Interfaces/"))
        {
            String iface = url.substring(16);
            if (iface == "eth")
                return getInterfaceEth(request);
            if (iface == "sta")
                return getInterfaceSta(request);
            if (iface == "wap")
                return getInterfaceWap(request);
        }
    }

    if (request->method() == HTTP_POST)
    {
        // Get a specific interface
        if (url.startsWith("/API/Interfaces/"))
        {
            String guid = url.substring(16);

            if (guid == "eth")
            {
                request->send(200, "text/plain", "Settings eth interface…");
                return;
            }
            if (guid == "sta")
            {
                request->send(200, "text/plain", "Settings sta interface…");
                return;
            }
        }
    }

    request->send(404);
}

void WebAPI::apiBodyHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    struct RequestContext
    {
        char buffer[512];
        size_t index = 0;
    };

    struct RequestContextGuard
    {
        AsyncWebServerRequest *req;
        RequestContext *ctx;

        ~RequestContextGuard()
        {
            delete ctx;
            req->_tempObject = nullptr;
        }
    };

    if (index == 0)
        request->_tempObject = new RequestContext();

    auto *ctx = (RequestContext *)request->_tempObject;

    if (!ctx)
    {
        request->send(500, "text/plain", "Internal error");
        return;
    }

    // Check if total size is not too big, + 1 for terminaison byte
    if (ctx->index + len + 1 >= sizeof(ctx->buffer))
    {
        request->send(413, "text/plain", "Payload too large");
        delete ctx;
        request->_tempObject = nullptr;
        return;
    }

    memcpy(&ctx->buffer[ctx->index], data, len);
    ctx->index += len;

    if (index + len != total)
        return;

    RequestContextGuard guard{request, ctx};

    ctx->buffer[ctx->index] = '\0';

    JsonDocument doc;
    if (deserializeJson(doc, ctx->buffer))
    {
        request->send(400, "text/plain", "Invalid JSON");
        return;
    }

    String path = request->url();

    // Remove /API/ prefix
    path.remove(0, 5);

#pragma region SplitPath
    std::vector<String> parts;
    int start = 0;
    int idx;

    while ((idx = path.indexOf('/', start)) != -1)
    {
        parts.push_back(path.substring(start, idx));
        start = idx + 1;
    }
    parts.push_back(path.substring(start));
#pragma endregion

    // Check if we have at least one part (the ressource type)
    if (parts.empty())
    {
        request->send(400, "text/plain", "Invalid URL");
        return;
    }

    // Dispatch
    if (parts[0] == "Variables")
    {
        // check if URL consists of 2 parts -> /API/Variables/{name}
        if (parts.size() != 2)
        {
            request->send(400, "text/plain", "Invalid URL");
            return;
        }

        // Check if name is provided
        if (parts[1].isEmpty())
        {
            request->send(400, "text/plain", "Variable name is required");
            return;
        }

        // Mandatory fields
        auto v = doc["value"];
        if (v.isUnbound() || v.isNull())
        {
            request->send(400, "text/plain", "Missing 'value' field");
            return;
        }

        MadMax::VariableValue value;
        if (!MadMax::VariableValueFromJson(v, value))
        {
            request->send(400, "text/plain", "Unsupported type");
            return;
        }

        // Set Variable value
        myApp->GetVariablesProvider()->SetValue(parts[1].c_str(), value);
        request->send(200, "text/plain", "OK");
        return;
    }

    if (parts[0] == "Accums")
    {
        // check if URL consists of 2 parts -> /API/Accums/{name}
        if (parts.size() != 2)
        {
            request->send(400, "text/plain", "Invalid URL");
            return;
        }

        // Check if name is provided
        if (parts[1].isEmpty())
        {
            request->send(400, "text/plain", "Accum name is required");
            return;
        }

        // Mandatory fields
        auto v = doc["value"];
        if (v.isUnbound() || v.isNull())
        {
            request->send(400, "text/plain", "Missing 'value' field");
            return;
        }

        MadMax::VariableValue value;
        if (!MadMax::VariableValueFromJson(v, value))
        {
            request->send(400, "text/plain", "Unsupported type");
            return;
        }

        // Set Variable value
        myApp->GetAccumsProvider()->SetValue(parts[1].c_str(), value);
        request->send(200, "text/plain", "OK");
        return;
    }

    request->send(404, "text/plain", "Ressource not found");

    /*
    String iface = url.startsWith("/API/Interfaces/") ? url.substring(16) : "";

    if (iface.isEmpty())
        return;

    if (index == 0)
        jsonIndex = 0;

    if (jsonIndex + len >= sizeof(jsonBuffer))
    {
        request->send(413, "text/plain", "Payload too large");
        return;
    }

    memcpy(&jsonBuffer[jsonIndex], data, len);
    jsonIndex += len;

    if (index + len == total)
    {
        jsonBuffer[jsonIndex] = '\0';

        if (iface == "eth")
            setInterfaceEth(jsonBuffer, request);
        else if (iface == "sta")
            setInterfaceSta(jsonBuffer, request);
        else
            request->send(404);
    }*/
}