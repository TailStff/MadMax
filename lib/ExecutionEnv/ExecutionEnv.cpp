// #define SERIALDEBUG

#include "ExecutionEnv.h"

#define PREFSAPPID "MadMaxApp"

ExecutionEnv::ExecutionEnv(unsigned int _cycle, RTCWrapper *rtcWrapper, Preferences *prefs, MiniPrefs *miniPrefs, Adafruit_SH1107 *display, mmModbusServerManager *modbusServerManager, std::function<void(IPConfigDhcp eth, IPConfigSTA sta, IPConfigWAP wap, String mDNS)> networkInit)
{
  cycle = _cycle;
  tickNumber = -1;
  lastOperatingTime = 0;
  this->rtcWrapper = rtcWrapper;
  this->modbusServerManager = modbusServerManager;
  this->prefs = prefs;
  this->miniPrefs = miniPrefs;
  this->display = display;

  GetfromPrefs();
  RetreiveHeapSize();

  this->networkInit = networkInit;

  heapSize = 0;
}

void ExecutionEnv::GetfromPrefs()
{
  prefs->begin(PREFSAPPID, true);

#ifdef SERIALDEBUG
  Serial.println("Reading Preferences data");
#endif

  mDNS = prefs->getString("mDNS", "madmax");

  ETH.Ip = GetIPAddressFromPrefs("ethIp", IPAddress(0, 0, 0, 0));
  ETH.Netmask = GetIPAddressFromPrefs("ethNetmask", IPAddress(0, 0, 0, 0));
  ETH.Gateway = GetIPAddressFromPrefs("ethGateway", IPAddress(0, 0, 0, 0));
  ETH.Dns1 = GetIPAddressFromPrefs("ethDns1", IPAddress(0, 0, 0, 0));
  ETH.Dns2 = GetIPAddressFromPrefs("ethDns2", IPAddress(0, 0, 0, 0));
  ETH.Dhcp = prefs->getBool("ethDhcp", true);

  STA.Ip = GetIPAddressFromPrefs("staIp", IPAddress(0, 0, 0, 0));
  STA.Netmask = GetIPAddressFromPrefs("staNetmask", IPAddress(0, 0, 0, 0));
  STA.Gateway = GetIPAddressFromPrefs("staGateway", IPAddress(0, 0, 0, 0));
  STA.Dns1 = GetIPAddressFromPrefs("staDns1", IPAddress(0, 0, 0, 0));
  STA.Dns2 = GetIPAddressFromPrefs("staDns2", IPAddress(0, 0, 0, 0));
  STA.SSID = prefs->getString("staSSID", "TailS");
  STA.Password = prefs->getString("staPassword", "T41l5l0v3r43v3r");
  STA.Dhcp = prefs->getBool("staDhcp", true);

  WAP.Ip = GetIPAddressFromPrefs("wapIp", IPAddress(192, 168, 11, 1));
  WAP.Netmask = GetIPAddressFromPrefs("wapNetmask", IPAddress(255, 255, 255, 0));
  WAP.Gateway = GetIPAddressFromPrefs("wapGateway", IPAddress(192, 168, 11, 1));
  WAP.SSID = prefs->getString("wapSSID", "AP_Madmax");
  WAP.Password = prefs->getString("wapPassword", "password");

#ifdef SERIALDEBUG
  Serial.println("Done reading Preferences data");
#endif

  prefs->end();
}

ExecutionEnv::~ExecutionEnv()
{
}

void ExecutionEnv::NetworksInitialization()
{
  networkInit(ETH, STA, WAP, mDNS);
}

int64_t ExecutionEnv::Execute()
{
  tickNumber++;
  rtcWrapper->RTCGet(dateTime);
  _currentMillis = Millis64::millis64();

  freeHeap = ESP.getFreeHeap();

  return tickNumber;
}

uint32_t ExecutionEnv::StopMetrics()
{
  lastOperatingTime = Millis64::millis64() - _currentMillis;
  return lastOperatingTime;
}

int64_t ExecutionEnv::GetTicks()
{
  return tickNumber;
}

uint16_t ExecutionEnv::GetCycle()
{
  return cycle;
}

uint8_t ExecutionEnv::getSecond()
{
  return dateTime.second();
}

uint8_t ExecutionEnv::getMinute()
{
  return dateTime.minute();
}

uint8_t ExecutionEnv::getHour()
{
  return dateTime.hour();
}

uint8_t ExecutionEnv::getDay()
{
  return dateTime.day();
}

uint8_t ExecutionEnv::getMonth()
{
  return dateTime.month();
}

uint16_t ExecutionEnv::getYear()
{
  return dateTime.year();
}

uint8_t ExecutionEnv::getDayOfWeek()
{
  return dateTime.dayOfTheWeek();
}

DateTime ExecutionEnv::getDateTime()
{
  return dateTime;
}

Preferences *ExecutionEnv::GetPrefs()
{
  return prefs;
}

MiniPrefs *ExecutionEnv::GetMiniPrefs()
{
  return miniPrefs;
}

mmModbusServerManager *ExecutionEnv::GetModbusServerManager()
{
  return modbusServerManager;
}

uint32_t ExecutionEnv::GetFreeHeap()
{
  return freeHeap;
}

uint32_t ExecutionEnv::GetHeapSize()
{
  return heapSize;
}

float ExecutionEnv::GetRamUsage()
{
  return (100.0f * (((float)heapSize - (float)freeHeap) / (float)heapSize));
}

void ExecutionEnv::RetreiveHeapSize()
{
  heapSize = ESP.getHeapSize();
}

Adafruit_SH1107 *ExecutionEnv::Display()
{
  return display;
}

IPConfigDhcp ExecutionEnv::GetActualETH() { return actualETH; }
IPConfigDhcp ExecutionEnv::GetActualSTA() { return actualSTA; }

void ExecutionEnv::RefreshActualEthInfo(IPAddress ipAddr, IPAddress netmaskAddr, IPAddress gwAddr, IPAddress dns1Addr, IPAddress dns2Addr)
{
  actualETH.Ip = ipAddr;
  actualETH.Netmask = netmaskAddr;
  actualETH.Gateway = gwAddr;
  actualETH.Dns1 = dns1Addr;
  actualETH.Dns2 = dns2Addr;
}

void ExecutionEnv::RefreshActualSTAInfo(IPAddress ipAddr, IPAddress netmaskAddr, IPAddress gwAddr, IPAddress dns1Addr, IPAddress dns2Addr)
{
  actualSTA.Ip = ipAddr;
  actualSTA.Netmask = netmaskAddr;
  actualSTA.Gateway = gwAddr;
  actualSTA.Dns1 = dns1Addr;
  actualSTA.Dns2 = dns2Addr;
}

IPAddress ExecutionEnv::GetIPAddressFromPrefs(const char *stringID, IPAddress defaultValue)
{
  byte bytes[4];
  if (prefs->getBytes(stringID, bytes, 4) == 4)
    return IPAddress(bytes);
  else
    return defaultValue;
}

void ExecutionEnv::PutIPAddressToPrefs(IPAddress ip, char *stringID)
{
  byte bytes[4];
  for (int i = 0; i < 4; i++)
    bytes[i] = ip[i];
  prefs->putBytes(stringID, bytes, 4);
}

void ExecutionEnv::SetETHProperties(IPConfigDhcp config)
{
#ifdef SERIALDEBUG
  Serial.println("Entering ExecutionEnv::SetETHProperties");
#endif

  ETH.Dhcp = config.Dhcp;
  ETH.Ip = config.Ip;
  ETH.Netmask = config.Netmask;
  ETH.Gateway = config.Gateway;
  ETH.Dns1 = config.Dns1;
  ETH.Dns2 = config.Dns2;

  prefs->begin(PREFSAPPID, false);

  prefs->putBool("ethDhcp", ETH.Dhcp);
  PutIPAddressToPrefs(ETH.Ip, "ethIp");
  PutIPAddressToPrefs(ETH.Netmask, "ethNetmask");
  PutIPAddressToPrefs(ETH.Gateway, "ethGateway");
  PutIPAddressToPrefs(ETH.Dns1, "ethDns1");
  PutIPAddressToPrefs(ETH.Dns2, "ethDns2");

  prefs->end();

#ifdef SERIALDEBUG
  Serial.println("Exiting ExecutionEnv::SetETHProperties");
#endif
}

void ExecutionEnv::SetSTAProperties(IPConfigSTA config)
{
#ifdef SERIALDEBUG
  Serial.println("Entering ExecutionEnv::SetSTAProperties");
#endif

  STA.Dhcp = config.Dhcp;
  STA.Ip = config.Ip;
  STA.Netmask = config.Netmask;
  STA.Gateway = config.Gateway;
  STA.Dns1 = config.Dns1;
  STA.Dns2 = config.Dns2;
  STA.SSID = config.SSID;
  STA.Password = config.Password;

  prefs->begin(PREFSAPPID, false);

  prefs->putBool("staDhcp", STA.Dhcp);
  PutIPAddressToPrefs(STA.Ip, "staIp");
  PutIPAddressToPrefs(STA.Netmask, "staNetmask");
  PutIPAddressToPrefs(STA.Gateway, "staGateway");
  PutIPAddressToPrefs(STA.Dns1, "staDns1");
  PutIPAddressToPrefs(STA.Dns2, "staDns2");
  prefs->putString("staSSID", STA.SSID);
  prefs->putString("staPassword", STA.Password);

  prefs->end();

#ifdef SERIALDEBUG
  Serial.println("Exiting ExecutionEnv::SetSTAProperties");
#endif
}

void ExecutionEnv::SetWAPProperties(IPConfigWAP config)
{
#ifdef SERIALDEBUG
  Serial.println("Entering ExecutionEnv::SetWAPProperties");
#endif

  WAP.Ip = config.Ip;
  WAP.Netmask = config.Netmask;
  WAP.Gateway = config.Gateway;
  WAP.SSID = config.SSID;
  WAP.Password = config.Password;

  prefs->begin(PREFSAPPID, false);

  PutIPAddressToPrefs(WAP.Ip, "wapIp");
  PutIPAddressToPrefs(WAP.Netmask, "wapNetmask");
  PutIPAddressToPrefs(WAP.Gateway, "wapGateway");
  prefs->putString("wapSSID", WAP.SSID);
  prefs->putString("wapPassword", WAP.Password);

  prefs->end();

#ifdef SERIALDEBUG
  Serial.println("Exiting ExecutionEnv::SetWAPProperties");
#endif
}

IPConfigDhcp ExecutionEnv::GetETHProperties() { return ETH; }
IPConfigSTA ExecutionEnv::GetSTAProperties() { return STA; }
IPConfigWAP ExecutionEnv::GetWAPProperties() { return WAP; }
