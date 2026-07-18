#include "WebAPI.h"
#include <LittleFS.h>

#include <ArduinoJson.h>
#include "VariableValue.h"

WebAPI::WebAPI(AsyncWebServer &server, MyApp *app, ExecutionEnv *env) : server(server), myApp(app), executionEnv(env) {}

/// Setup the web server routes and handlers
void WebAPI::Setup()
{
    // API
    server.on("/API/*", HTTP_GET, [this](AsyncWebServerRequest *r)
              { handleAPI(r); });

    server.on("/API/*", HTTP_POST, [this](AsyncWebServerRequest *r)
              { handleAPI(r); }, NULL, [this](AsyncWebServerRequest *r, uint8_t *data, size_t len, size_t index, size_t total)
              { apiBodyHandler(r, data, len, index, total); });

    /*
    server.on("/INFOS", HTTP_GET, SendInfos);
    server.on("/DATAS", HTTP_GET, SendDatas);
    server.on("/RESET", HTTP_GET, ESPReset);
    server.on("/IP", HTTP_POST, setIPAddressResponse, NULL, setIPAddressExecute);
    server.on("/SETTINGS", HTTP_POST, setSettingsResponse, NULL, setSettingsExecute);
    server.on("/DATETIME", HTTP_POST, setDateTimeResponse, NULL, setDateTimeExecute);*/

    // server.onNotFound(notFound);

    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

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
            else if (parts[0] == "Interfaces")
                return getInterfacesList(request);
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
            else if (parts[0] == "Interfaces")
            {
                if (parts[1] == "eth")
                    return getInterfaceEth(request);
                else if (parts[1] == "sta")
                    return getInterfaceSta(request);
                else if (parts[1] == "wap")
                    return getInterfaceWap(request);
            }
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

            if (parts[0] == "Interfaces")
            {
                // We check Interface existence before reading the body, to avoid reading a potentially big body if the Interface doesn't exist or is not writable
                if (parts[1] == "eth" || parts[1] == "sta" || parts[1] == "wap")
                    return;
            }
        }
        else if (parts.size() == 4)
        {
            if (parts[0] == "PumpSwaps")
            {
                if (parts[2] == "actions")
                    return triggerPumpSwapAction(parts[1].c_str(), parts[3].c_str(), request);
            }
        }
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

#pragma region PumpSwap
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

void WebAPI::triggerPumpSwapAction(const std::string &name, const std::string &action, AsyncWebServerRequest *request)
{
    MadMax::DTOBase dto;
    if (myApp->GetPumpSwapProvider()->TriggerAction(name.c_str(), action.c_str(), dto))
    {
        request->send(200, "text/plain", "OK");
    }
    else
    {
        request->send(404, "text/plain", "Object not found");
    }
}
#pragma endregion PumpSwap

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

// Interfaces
#pragma region Interfaces

void WebAPI::getInterfacesList(AsyncWebServerRequest *request)
{
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    std::vector<std::string> _list = {"ETH", "STA", "WAP"};

    for (const auto &dto : _list)
    {
        JsonObject obj = arr.add<JsonObject>();
        obj["name"] = dto;

        if (dto == "ETH")
            getInterfaceEthProperties(obj);
        else if (dto == "STA")
            getInterfaceStaProperties(obj);
        else if (dto == "WAP")
            getInterfaceWapProperties(obj);
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
}

void WebAPI::getInterfaceEth(AsyncWebServerRequest *request)
{
    JsonDocument doc;
    JsonObject obj = doc.to<JsonObject>();

    obj["name"] = "ETH";
    getInterfaceEthProperties(obj);

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
}

void WebAPI::getInterfaceSta(AsyncWebServerRequest *request)
{
    JsonDocument doc;
    JsonObject obj = doc.to<JsonObject>();

    obj["name"] = "STA";
    getInterfaceStaProperties(obj);

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
}

void WebAPI::getInterfaceWap(AsyncWebServerRequest *request)
{
    JsonDocument doc;
    JsonObject obj = doc.to<JsonObject>();

    obj["name"] = "WAP";
    getInterfaceWapProperties(obj);

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
}

void WebAPI::getInterfaceEthProperties(JsonObject obj)
{
    auto eth = executionEnv->GetETHProperties();

    obj["dhcp"] = eth.Dhcp;
    obj["ip"] = eth.Ip.toString();
    obj["netmask"] = eth.Netmask.toString();
    obj["gateway"] = eth.Gateway.toString();
    obj["dns1"] = eth.Dns1.toString();
    obj["dns2"] = eth.Dns2.toString();
}

void WebAPI::getInterfaceStaProperties(JsonObject obj)
{
    auto sta = executionEnv->GetSTAProperties();

    obj["dhcp"] = sta.Dhcp;
    obj["ip"] = sta.Ip.toString();
    obj["netmask"] = sta.Netmask.toString();
    obj["gateway"] = sta.Gateway.toString();
    obj["dns1"] = sta.Dns1.toString();
    obj["dns2"] = sta.Dns2.toString();
    obj["ssid"] = sta.SSID.c_str();
    obj["password"] = sta.Password.c_str();
}

void WebAPI::getInterfaceWapProperties(JsonObject obj)
{
    auto wap = executionEnv->GetWAPProperties();

    obj["ip"] = wap.Ip.toString();
    obj["netmask"] = wap.Netmask.toString();
    obj["gateway"] = wap.Gateway.toString();
    obj["ssid"] = wap.SSID.c_str();
    obj["password"] = wap.Password.c_str();
}

void WebAPI::setInterfaceEth(JsonDocument doc, AsyncWebServerRequest *request)
{
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

    executionEnv->SetRestart();
}

void WebAPI::setInterfaceSta(JsonDocument doc, AsyncWebServerRequest *request)
{
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

    executionEnv->SetSTAProperties(config);
    request->send(200, "text/plain", "OK");

    executionEnv->SetRestart();
}

void WebAPI::setInterfaceWap(JsonDocument doc, AsyncWebServerRequest *request)
{
    IPAddress ip, netmask, gateway;
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

    IPConfigWAP config;
    config.Ip = ip;
    config.Netmask = netmask;
    config.Gateway = gateway;
    config.SSID = doc["ssid"].as<String>();
    config.Password = doc["password"].as<String>();

    executionEnv->SetWAPProperties(config);
    request->send(200, "text/plain", "OK");

    executionEnv->SetRestart();
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

#pragma endregion Interfaces

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

    if (parts[0] == "Interfaces")
    {
        // check if URL consists of 2 parts -> /API/Interfaces/{name}
        if (parts.size() != 2)
        {
            request->send(400, "text/plain", "Invalid URL");
            return;
        }

        // Check if name is provided
        if (parts[1].isEmpty())
        {
            request->send(400, "text/plain", "Interface name is required");
            return;
        }

        if (parts[1] == "eth")
        {
            setInterfaceEth(doc, request);
            request->send(200, "text/plain", "OK");
            return;
        }
        else if (parts[1] == "sta")
        {
            setInterfaceSta(doc, request);
            request->send(200, "text/plain", "OK");
            return;
        }
        else if (parts[1] == "wap")
        {
            setInterfaceWap(doc, request);
            request->send(200, "text/plain", "OK");
            return;
        }
        else
        {
            request->send(400, "text/plain", "Unsupported interface");
            return;
        }
    }

    request->send(404, "text/plain", "Ressource not found");
}