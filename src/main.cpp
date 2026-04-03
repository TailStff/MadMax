// #define OLED_SSD1306
#define OLED_SH1107

// #define SERIALDEBUG

#define GPIO0 0
#define PIN_SDA 4
#define PIN_SCL 5

#define PIN_SDA2 32
#define PIN_SCL2 33

#define PIN_RS485_RX 16
#define PIN_RS485_TX 13
#define RS485_BAUDRATE 19200
#define RS485_PARITY SERIAL_8E1

#define ETH_PHY_TYPE ETH_PHY_LAN8720
#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT
#define ETH_PHY_MDC 23
#define ETH_PHY_MDIO 18
#define ETH_PHY_POWER -1
#define ETH_PHY_ADDR 0

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels

#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32 (Here my OLED 128x64 SSD1306 is 0x3C ^^')

#define MMCYCLE 100 // Define application cycle duration in ms

#include <Arduino.h>
#include <stdexcept>
#include <ArduinoJson.h>
#include <PCF8574.h>
#include <Preferences.h>
#include "Adafruit_FRAM_I2C.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ETH.h>
#include <SPI.h>

#include <Adafruit_GFX.h>

// #include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>

#include "MiniPrefs.h"

#include "millis64.h"
#include "SafeSet.h"

#include "MadMax.h"

#include "home.h"
#include "styles.css.h"

#include "RTCWrapper.h"

#include "IntervalCallback.h"
#include "myApp.h"
#include <DigitalInputs.h>
#include <DigitalOutputs.h>
#include "ExecutionEnv.h"

#include "WebAPI.h"

#pragma region Modbus Server
#include "ModbusServerMemoryManager.h"
#include "ModbusServerManager.h"

const uint16_t MBserver_Port = 502;      // Port number the server shall listen to
const uint16_t MBserver_MaxClient = 2;   // Max clients connected at the same time
const uint16_t MBserver_Timeout = 20000; // Timeout

MadMax::ModbusServerMemoryManager modbusServerMemoryManager;
MadMax::ModbusServerManager mbServerManager(modbusServerMemoryManager);
#pragma endregion

#include "main.h"

ExecutionEnv *executionEnv;
MyApp *myApp;
WebAPI *webAPI = nullptr;

void networkOnEvent(arduino_event_id_t event, arduino_event_info_t info);

// Interval to set data memory values
IntervalCallback intervalCallback(callbackExecution);

// Object used to MANAGE Digital Inputs
DigitalInputs digitalInputs;

// Object used to MANAGE Digital Outputs
DigitalOutputs digitalOutputs;

// MadMax Global variables

// Global variable used to indicate if ETH is connected
static bool eth_connected = false;

// Default IP values
IPAddress WAP_ipAddress;
IPAddress WAP_ipGateway;
IPAddress WAP_ipMask;

String WSTA_SSID;
String WSTA_pwd;

/* Global WebServeur instance */
// WebServer server(80);
AsyncWebServer server(80);

// First I2C bus used for Digital Input/Output / I2C internal port
TwoWire I2Cone = TwoWire(0);
// Second I2C bus used for ??
TwoWire I2Ctwo = TwoWire(1);

// Old version of board
PCF8574 pcf8574_I1(&I2Cone, 0x22, PIN_SDA, PIN_SCL);
PCF8574 pcf8574_I2(&I2Cone, 0x21, PIN_SDA, PIN_SCL);
PCF8574 pcf8574_R1(&I2Cone, 0x24, PIN_SDA, PIN_SCL);
PCF8574 pcf8574_R2(&I2Cone, 0x25, PIN_SDA, PIN_SCL);

RTCWrapper rtcWrapper(&I2Cone);

// Preferences object
Preferences prefs;

// I2C mini prefs object
// I2CMiniPrefs myPrefs(MEM_TYPE_FRAM, 0x50, 256L * 1024L, 256, 33, 120, &I2Cone);

// FRAM fram(&I2Cone);

Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();
MiniPrefs *myPrefs;

// Create char buffer for JSON serialization or string concatenation
char message[128];

class MyModbusClientRTU : public ModbusClientRTU
{
public:
  using ModbusClientRTU::ModbusClientRTU;

  void setQueueLimit(uint16_t limit)
  {
    MR_qLimit = limit;
  }
};

MyModbusClientRTU MBRTU(Serial2);

// OLED screen global object
#ifdef OLED_SSD1306
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2Cone, OLED_RESET);
#endif

#ifdef OLED_SH1107
Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &I2Cone, OLED_RESET, 1000000, 1000000);
#endif

void onNetworkInit(IPConfigDhcp eth, IPConfigSTA sta, IPConfigWAP wap, String mDNS)
{
#ifdef SERIALDEBUG
  Serial.println("Network initialization");
#endif

  WiFi.onEvent(networkOnEvent);

  ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE);

  if (eth.Dhcp)
    ETH.config(IPAddress(0, 0, 0, 0), IPAddress(0, 0, 0, 0), IPAddress(0, 0, 0, 0));
  else
    ETH.config(eth.Ip, eth.Gateway, eth.Netmask, eth.Dns1, eth.Dns2);

  WiFi.mode(WIFI_MODE_APSTA);

  WiFi.softAP(wap.SSID.c_str(), wap.Password.c_str());
  WiFi.softAPConfig(wap.Ip, wap.Gateway, wap.Netmask);

  WiFi.begin(sta.SSID.c_str(), sta.Password.c_str());

  // Start Multicast DNS
  MDNS.begin(mDNS);
}

void networkOnEvent(arduino_event_id_t event, arduino_event_info_t info)
{

  switch (event)
  {

  case ARDUINO_EVENT_ETH_START:
#ifdef SERIALDEBUG
    Serial.println("ETH Started");
#endif
    // The hostname must be set after the interface is started, but needs
    // to be set before DHCP, so set it from the event handler thread.
    ETH.setHostname("esp32-ethernet");
    break;

  case ARDUINO_EVENT_ETH_CONNECTED:
#ifdef SERIALDEBUG
    Serial.println("ETH Connected");
#endif
    break;

  case ARDUINO_EVENT_ETH_GOT_IP:
#ifdef SERIALDEBUG
    Serial.printf("ETH Got IP: '%s'\n", esp_netif_get_desc(info.got_ip.esp_netif));
#endif

    executionEnv->RefreshActualEthInfo(info.got_ip.ip_info.ip.addr, info.got_ip.ip_info.netmask.addr, info.got_ip.ip_info.gw.addr, ETH.dnsIP(0), ETH.dnsIP(1));

    eth_connected = true;
    rtcWrapper.SyncDateTimeFromNTP();
    break;

  case ARDUINO_EVENT_ETH_DISCONNECTED:
#ifdef SERIALDEBUG
    Serial.println("ETH Disconnected");
#endif

    executionEnv->RefreshActualEthInfo(IPAddress(0, 0, 0, 0), IPAddress(0, 0, 0, 0), IPAddress(0, 0, 0, 0), IPAddress(0, 0, 0, 0), IPAddress(0, 0, 0, 0));

    eth_connected = false;
    break;

  case ARDUINO_EVENT_ETH_STOP:
#ifdef SERIALDEBUG
    Serial.println("ETH Stopped");
#endif
    eth_connected = false;
    break;

  case ARDUINO_EVENT_WIFI_STA_GOT_IP:
#ifdef SERIALDEBUG
    Serial.printf("STA Got IP: '%s'\n", esp_netif_get_desc(info.got_ip.esp_netif));
#endif
    executionEnv->RefreshActualSTAInfo(info.got_ip.ip_info.ip.addr, info.got_ip.ip_info.netmask.addr, info.got_ip.ip_info.gw.addr, WiFi.dnsIP(0), WiFi.dnsIP(1));
    break;

  default:
    break;
  }
}

void SetupInputsOutputs()
{

  // Set PCF8574 pinMode to OUTPUT
  for (int i = 0; i <= 7; i++)
  {
    pcf8574_R1.pinMode(i, OUTPUT);
    pcf8574_R2.pinMode(i, OUTPUT);
  }

  pcf8574_R1.begin();
  pcf8574_R2.begin();

  for (int i = 0; i <= 7; i++)
  {
    pcf8574_I1.pinMode(i, INPUT);
    pcf8574_I2.pinMode(i, INPUT);
  }

  pcf8574_I1.begin();
  pcf8574_I2.begin();

  for (int i = 0; i <= 7; i++)
  {
    pcf8574_R1.digitalWrite(i, HIGH);
    pcf8574_R2.digitalWrite(i, HIGH);
  }
}

/*
// Define an onData handler function to receive the regular responses
// Arguments are Modbus server ID, the function code requested, the message data and length of it,
// plus a user-supplied token to identify the causing request
void handleData(ModbusMessage response, uint32_t token)
{
  // Only print out result of the "real" example - not the request preparing the field

  Serial.println("Response: serverID=" + String(response.getServerID()) + ", FC=" + String(response.getFunctionCode()) + ", Token=" + String(token, HEX) + ", length=" + String(response.size()));
  uint8_t byteCount = response[2];
  for (int i = 0; i < byteCount / 2; i++)
  {

    uint16_t reg = (response[3 + i * 2] << 8) | response[4 + i * 2];

    Serial.printf("Reg %d = %u\n", i, reg);
  }
}

// Define an onError handler function to receive error responses
// Arguments are the error code returned and a user-supplied token to identify the causing request
void handleError(Error error, uint32_t token)
{
  // ModbusError wraps the error code and provides a readable error message for it
  ModbusError me(error);
  Serial.println("Error response: " + String((int)me, HEX) + " - " + String((const char *)me));
}*/

// Global variables
// there are mirroring variables in the Modbus server holding registers, which are updated in the callbackExecution function, and can be read by Modbus clients.
int64_t *myTick = nullptr;
uint16_t *y = nullptr;
uint16_t *m = nullptr;
uint16_t *d = nullptr;
uint16_t *h = nullptr;
uint16_t *mn = nullptr;
uint16_t *s = nullptr;

void setup()
{
#ifdef SERIALDEBUG
  Serial.println("Entering main::setup() function");
#endif

  Serial.begin(115200);

  pinMode(GPIO0, INPUT_PULLUP);

  // Initialise I2C bus
  I2Cone.begin(PIN_SDA, PIN_SCL);
  // speeds are 10000, 100000, 400000, 1000000
  I2Cone.setClock(1000000);

  // Initialise I2C bus
  I2Ctwo.begin(PIN_SDA2, PIN_SCL2);
  // speeds are 10000, 100000, 400000, 1000000
  I2Ctwo.setClock(1000000);

  if (fram.begin(0x50, &I2Ctwo))
  {
#ifdef SERIALDEBUG
    Serial.println("FRAM initialized successfully");
#endif
  }
  else
  {
    Serial.println("ERROR while initializing FRAM");
  }

  delay(1000);

  myPrefs = new MiniPrefs(fram, 32 * 1024);
  myPrefs->begin();

  executionEnv = new ExecutionEnv(MMCYCLE, &rtcWrapper, &prefs, myPrefs, &display, &modbusServerMemoryManager, onNetworkInit);

#ifdef SERIALDEBUG
  Serial.println("ExecutionEnv object created");
#endif

  executionEnv->NetworksInitialization();

#ifdef SERIALDEBUG
  Serial.println("ExecutionEnv::NetworksInitialization() executed");
#endif

  myApp = new MyApp(executionEnv, MBRTU, &modbusServerMemoryManager, &digitalInputs, &digitalOutputs);

#ifdef SERIALDEBUG
  Serial.println("MyApp object created");
#endif

  myTick = modbusServerMemoryManager.AssociateHoldingRegister<int64_t>(0);
  y = modbusServerMemoryManager.AssociateHoldingRegister<uint16_t>(4);
  m = modbusServerMemoryManager.AssociateHoldingRegister<uint16_t>(5);
  d = modbusServerMemoryManager.AssociateHoldingRegister<uint16_t>(6);
  h = modbusServerMemoryManager.AssociateHoldingRegister<uint16_t>(7);
  mn = modbusServerMemoryManager.AssociateHoldingRegister<uint16_t>(8);
  s = modbusServerMemoryManager.AssociateHoldingRegister<uint16_t>(9);

  RTUutils::prepareHardwareSerial(Serial2);
  Serial2.begin(RS485_BAUDRATE, RS485_PARITY, PIN_RS485_RX, PIN_RS485_TX);

#ifdef SERIALDEBUG
  Serial.println("RS485 interface setted successfully");
#endif

  // Set up ModbusRTU client.
  MBRTU.setTimeout(100);
  MBRTU.setQueueLimit(200);
  MBRTU.begin(Serial2);

#ifdef SERIALDEBUG
  Serial.println("RS485 client setted successfully");
#endif

  /*
    I2Cone.beginTransmission(0x50);
    for (uint16_t addr = 0; addr < 32768; addr++)
    {
      I2Cone.write(0xFF);
    }
    I2Cone.endTransmission();*/

  // Serial.println(myPrefs.putString("applicationName", "MadMax"));

  rtcWrapper.begin();

#ifdef OLED_SSD1306
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
#endif

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.

#ifdef OLED_SH1107
  display.begin(SCREEN_ADDRESS, true); // Address 0x3D default
  display.setContrast(0x7F);           // dim display
#endif

  display.display();
  // delay(2000); // Pause for 2 seconds, display default logo

  display.clearDisplay();
  display.setTextSize(1);

#ifdef OLED_SSD1306
  display.setTextColor(SSD1306_WHITE);
#endif

#ifdef OLED_SH1107
  display.setTextColor(SH110X_WHITE);
#endif

  display.display();

  // Prepare hardware to manage Inputs and Outputs
  SetupInputsOutputs();

  mbServerManager.RegisterWorkers();
  mbServerManager.Start(MBserver_Port, MBserver_MaxClient, MBserver_Timeout);

#ifdef SERIALDEBUG
  Serial.println("Modbus server successfully started");
#endif

  webAPI = new WebAPI(server, myApp, executionEnv);
  webAPI->Setup();

#ifdef SERIALDEBUG
  Serial.println("Web server successfully created");
#endif

  // Retreive heap size long time after start because heap size computation didn't take all heap size
  executionEnv->RetreiveHeapSize();

  intervalCallback.Start(MMCYCLE, true);

  myApp->Init();

#ifdef SERIALDEBUG
  Serial.println("MyApp::Init() executed");
#endif
}

void loop()
{
  intervalCallback.Tick();
  delay(10);
}

void callbackExecution()
{
  try
  {
    // Increment execution ticks

    // Inputs refresh ////////////////////////////////////////////////////////////////////////////
    digitalInputs.RefreshDigitalInputs(pcf8574_I1, pcf8574_I2, *modbusServerMemoryManager.GetCoilsPtr());

    // Start of Application //////////////////////////////////////////////////////////////////////

    myApp->Loop();

    // End of Application ////////////////////////////////////////////////////////////////////////

    SafeSet<int64_t>(myTick, executionEnv->GetTicks());
    SafeSet<uint16_t>(y, (uint16_t)executionEnv->getYear());
    SafeSet<uint16_t>(m, (uint16_t)executionEnv->getMonth());
    SafeSet<uint16_t>(d, (uint16_t)executionEnv->getDay());
    SafeSet<uint16_t>(h, (uint16_t)executionEnv->getHour());
    SafeSet<uint16_t>(mn, (uint16_t)executionEnv->getMinute());
    SafeSet<uint16_t>(s, (uint16_t)executionEnv->getSecond());

    // Outputs refresh ///////////////////////////////////////////////////////////////////////////
    digitalOutputs.RefreshDigitalOutputs(pcf8574_R1, pcf8574_R2, *modbusServerMemoryManager.GetCoilsPtr());
  }
  catch (const std::exception &ex)
  {
    // Exception was thrown
    Serial.println(F("PANIC"));

    // Set all outputs to Off
    for (int i = 0; i < 16; i++)
    {
      digitalOutputs.Set(i, false);
    }
    // Outputs refresh
    digitalOutputs.RefreshDigitalOutputs(pcf8574_R1, pcf8574_R2, *modbusServerMemoryManager.GetCoilsPtr());

    // Stop the callback
    intervalCallback.Stop();
  }
}