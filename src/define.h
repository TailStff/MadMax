// #define OLED_SSD1306
#define OLED_SH1107

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
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32, 0x3C for 128x128 (SH1107)

#define MMCYCLE 100 // Define application cycle duration in ms

#define MODBUS_MAXCOILS 10000
#define MODBUS_MAXDISCRETEINPUTS 10000
#define MODBUS_MAXINPUTREGISTERS 10000
#define MODBUS_MAXHOLDINGREGISTERS 10000