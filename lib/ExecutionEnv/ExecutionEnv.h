#ifndef EXECUTIONENV
#define EXECUTIONENV

#include <stdio.h>
#include <HardwareSerial.h>
#include <Preferences.h>
#include "MiniPrefs.h"
#include "ModbusServerManager.h"
#include "ModbusServerMemoryManager.h"

#include "helpersIPAddress.h"

#include "millis64.h"

#include <Adafruit_SH110X.h>

#include "RTCWrapper.h"

struct IPConfigWAP
{
  IPAddress Ip;
  IPAddress Netmask;
  IPAddress Gateway;
  String SSID;
  String Password;
};

struct IPConfig
{
  IPAddress Ip;
  IPAddress Netmask;
  IPAddress Gateway;
  IPAddress Dns1;
  IPAddress Dns2;
};

struct IPConfigDhcp : public IPConfig
{
  bool Dhcp;
};

struct IPConfigSTA : public IPConfigDhcp
{
  String SSID;
  String Password;
};

class ExecutionEnv
{
private:
  // Flag to indicate if a restart is pending
  bool RestartPending = false;

  // Store the number of cycles of the application
  int64_t tickNumber;

  // Store the last execution duration in ms
  uint32_t lastOperatingTime;

  // Store the cycle duration in ms
  uint16_t cycle;

  uint64_t _currentMillis;

  uint32_t freeHeap = 0;
  uint32_t heapSize = 0;

  RTCWrapper *rtcWrapper;

  DateTime dateTime;

  Preferences *prefs;
  MiniPrefs *miniPrefs;

  Adafruit_SH1107 *display;

  MadMax::ModbusServerManager *modbusServerManager;
  MadMax::ModbusServerMemoryManager *modbusServerMemoryManager;

  /// @brief Callback function for Initialize network interfaces
  std::function<void(IPConfigDhcp eth, IPConfigSTA sta, IPConfigWAP wap, String mDNS)> networkInit;
  /// @brief  Configuration object for ETH interface
  IPConfigDhcp ETH;
  /// @brief  Configuration object for Wifi station/client interface
  IPConfigSTA STA;
  /// @brief Configuration object for Wifi access point interface
  IPConfigWAP WAP;

  String mDNS;

  IPConfigDhcp actualETH;
  IPConfigDhcp actualSTA;

  void GetfromPrefs();
  IPAddress GetIPAddressFromPrefs(const char *stringID, IPAddress defaultValue);
  void PutIPAddressToPrefs(IPAddress ip, char *stringID);

public:
  // Constructor
  ExecutionEnv(unsigned int _cycle, RTCWrapper *rtcWrapper, Preferences *prefs, MiniPrefs *miniPrefs, Adafruit_SH1107 *display, MadMax::ModbusServerManager *modbusServerManager, MadMax::ModbusServerMemoryManager *modbusServerMemoryManager, std::function<void(IPConfigDhcp eth, IPConfigSTA sta, IPConfigWAP wap, String mDNS)> networkInit);

  // Destructor
  ~ExecutionEnv();

  bool isRestartPending() { return RestartPending; }
  void SetRestart() { RestartPending = true; }

  void RetreiveHeapSize();

  void NetworksInitialization();

  // Function to call when starting a new cycle
  int64_t Execute();

  // Function to call when application cycle is done (to calculate duration stored in lastOperatingTime)
  uint32_t StopMetrics();

  // Function that return current Tick (marked as virtual as unitary test could override it)
  int64_t virtual GetTicks();

  // Function that return cycle length in ms (marked as virtual as unitary test could override it)
  uint16_t virtual GetCycle();

  // Get preferences object
  Preferences *GetPrefs();

  MiniPrefs *GetMiniPrefs();

  MadMax::ModbusServerManager *GetModbusServerManager();
  MadMax::ModbusServerMemoryManager *GetModbusServerMemoryManager();

  // Get I2C mini prefs object
  // Adafruit_FRAM_I2C *GetMiniPrefs() { return this->fram; }

  uint8_t getSecond();
  uint8_t getMinute();
  uint8_t getHour();
  uint8_t getDay();
  uint8_t getMonth();
  uint16_t getYear();
  uint8_t getDayOfWeek();
  DateTime getDateTime();
  uint32_t GetFreeHeap();
  uint32_t GetHeapSize();
  float GetRamUsage();

  Adafruit_SH1107 *Display();

  IPConfigDhcp GetActualETH();
  IPConfigDhcp GetActualSTA();

  void RefreshActualEthInfo(IPAddress ipAddr, IPAddress netmaskAddr, IPAddress gwAddr, IPAddress dns1Addr, IPAddress dns2Addr);
  void RefreshActualSTAInfo(IPAddress ipAddr, IPAddress netmaskAddr, IPAddress gwAddr, IPAddress dns1Addr, IPAddress dns2Addr);

  IPConfigDhcp GetETHProperties();
  IPConfigSTA GetSTAProperties();
  IPConfigWAP GetWAPProperties();

  void SetETHProperties(IPConfigDhcp config);
  void SetSTAProperties(IPConfigSTA config);
  void SetWAPProperties(IPConfigWAP config);
};

#endif