#include "WebAPI.h"
#include "home.h"
#include "styles.css.h"
#include <ArduinoJson.h>
#include "VariableValue.h"

WebAPI::WebAPI(AsyncWebServer &server, MyApp *app, ExecutionEnv *env) : server(server), myApp(app), executionEnv(env) {}

void WebAPI::Setup()
{

    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *r)
              { homePage(r); });
    server.on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *r)
              { homePage(r); });

    server.on("/styles.css", HTTP_GET, [this](AsyncWebServerRequest *r)
              { styles(r); });

    server.on("^/API/Variables$", HTTP_GET,
              [this](AsyncWebServerRequest *r)
              { getVariablesList(r); });

    server.on("^/API/Accums$", HTTP_GET,
              [this](AsyncWebServerRequest *r)
              { getAccumsList(r); });

    server.on("^/API/DigitalEquipments$", HTTP_GET,
              [this](AsyncWebServerRequest *r)
              { getDigitalEquipmentsList(r); });

    server.on("^/API/PumpSwaps$", HTTP_GET,
              [this](AsyncWebServerRequest *r)
              { getPumpSwapsList(r); });

    server.on("^/API/TPulses$", HTTP_GET,
              [this](AsyncWebServerRequest *r)
              { getTPulsesList(r); });

    server.on("^/API/FeedbackErrors$", HTTP_GET,
              [this](AsyncWebServerRequest *r)
              { getFeedbackErrorsList(r); });

    server.on("^/API/DelayOnOffs$", HTTP_GET,
              [this](AsyncWebServerRequest *r)
              { getDelayOnOffsList(r); });

    server.on("^/API/Variables/([^/]+)$", HTTP_GET,
              [this](AsyncWebServerRequest *r)
              { getVariableDetail(r->pathArg(0), r); });

    server.on("^/API/Accums/([^/]+)$", HTTP_GET,
              [this](AsyncWebServerRequest *r)
              { getAccumDetail(r->pathArg(0), r); });

    server.on("^/API/DigitalEquipments/([^/]+)$", HTTP_GET,
              [this](AsyncWebServerRequest *r)
              { getDigitalEquipmentDetail(r->pathArg(0), r); });

    server.on("^/API/PumpSwaps/([^/]+)$", HTTP_GET,
              [this](AsyncWebServerRequest *r)
              { getPumpSwapDetail(r->pathArg(0), r); });

    server.on("^/API/TPulses/([^/]+)$", HTTP_GET,
              [this](AsyncWebServerRequest *r)
              { getTPulseDetail(r->pathArg(0), r); });

    server.on("^/API/FeedbackErrors/([^/]+)$", HTTP_GET,
              [this](AsyncWebServerRequest *r)
              { getFeedbackErrorDetail(r->pathArg(0), r); });

    server.on("^/API/DelayOnOffs/([^/]+)$", HTTP_GET,
              [this](AsyncWebServerRequest *r)
              { getDelayOnOffDetail(r->pathArg(0), r); });

    server.on("^/API/Variables/([^/]+)/([^/]+)$", HTTP_GET,
              [this](AsyncWebServerRequest *r)
              { getVariableProperty(r->pathArg(0).c_str(), r->pathArg(1).c_str(), r); });

    server.on("/API", HTTP_GET, [this](AsyncWebServerRequest *r)
              { apiRouter(r); });

    server.on("/API", HTTP_POST, [this](AsyncWebServerRequest *r)
              { apiRouter(r); }, NULL, [this](AsyncWebServerRequest *r, uint8_t *data, size_t len, size_t index, size_t total)
              { apiBodyHandler(r, data, len, index, total); });

    // server.on("/API/interfaces", HTTP_GET, apiGetInterfaces);

    // server.on("/API/interfaces/:int", HTTP_GET, apiGetInterface);

    /*server.on("/app.js", HTTP_GET, application);
    server.on("/styles.css", HTTP_GET, styles);
    server.on("/INFOS", HTTP_GET, SendInfos);
    server.on("/DATAS", HTTP_GET, SendDatas);
    server.on("/RESET", HTTP_GET, ESPReset);
    server.on("/IP", HTTP_POST, setIPAddressResponse, NULL, setIPAddressExecute);
    server.on("/SETTINGS", HTTP_POST, setSettingsResponse, NULL, setSettingsExecute);
    server.on("/DATETIME", HTTP_POST, setDateTimeResponse, NULL, setDateTimeExecute);*/

    // server.onNotFound(notFound);

    server.begin();
}

// -------- Pages --------

void WebAPI::homePage(AsyncWebServerRequest *request)
{
    request->send_P(200, "text/html", Home);
}

void WebAPI::styles(AsyncWebServerRequest *request)
{
    request->send_P(200, "text/css", Styles);
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

    // Objets enfants
    for (const auto &childDto : dto.children)
    {
        JsonObject child = obj.createNestedObject(childDto.objectName);
        WriteJson(child, childDto);
    }
}

// -------- Variables --------

void WebAPI::getVariablesList(AsyncWebServerRequest *request)
{
    auto variables = myApp->GetVariableProvider()->GetDTOs();

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

void WebAPI::getVariableDetail(const String &name, AsyncWebServerRequest *request)
{
    MadMax::DTOBase dto;
    if (myApp->GetVariableProvider()->GetDetailDTO(name.c_str(), dto))
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
    if (!myApp->GetVariableProvider()->GetDetailDTO(name.c_str(), dto))
    {
        request->send(404, "text/plain", "Object not found");
        return;
    }

    JsonDocument doc;

    auto it = std::find_if(dto.fields.begin(), dto.fields.end(), [&](const MadMax::FieldValue &f)
                           { return f.key == property; });

    JsonObject obj = doc.as<JsonObject>();

    if (it != dto.fields.end())
    {
        WriteJson(obj, it[0]);
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

void WebAPI::getAccumDetail(const String &name, AsyncWebServerRequest *request)
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

void WebAPI::getDigitalEquipmentDetail(const String &name, AsyncWebServerRequest *request)
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

// Digital Equipments
void WebAPI::getPumpSwapsList(AsyncWebServerRequest *request)
{
    auto ps = myApp->GetPumpSwapProvider()->GetDTOs();

    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    for (const auto &dto : ps)
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

void WebAPI::getPumpSwapDetail(const String &name, AsyncWebServerRequest *request)
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

void WebAPI::getTPulseDetail(const String &name, AsyncWebServerRequest *request)
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

void WebAPI::getFeedbackErrorDetail(const String &name, AsyncWebServerRequest *request)
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

// DelayOnOff Errors
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

void WebAPI::getDelayOnOffDetail(const String &name, AsyncWebServerRequest *request)
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
    static char jsonBuffer[512];
    static size_t jsonIndex;

    String url = request->url();
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
    }
}