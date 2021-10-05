// Load Wi-Fi library
#include <WiFi.h>

//Celestron NexStar 127 SLT WiFi control module.
//I made this module to connect my telescope to my iPad and control it with SkySafari Pro 6
//It can also be used to control it with a laptop, using K-Stars, or Stellarium.
//The ESP32 board creates a WiFi access point and a TCP server on port 4030, and it creates a bridge to the serial port, forwarding all the data in both directions.
//This is the default port for "SkyFi" on "SkySafari Pro 6" and it should work with other telescopes as well.
//The serial port's U0 Tx and Rx are then connected to the telescope's Hand Controller through an RS232 TTL 3.3V level converter.
//Be careful: The AUX port of the telescope, where the hand controller connects, is 5V CMOS compatible.
//Also, the serial protocol used by the Hand Controller's telescope to communicate with the mount, is different than the one the hand controller is expecting on its RS232 PC port.
//On my iPad, on SkySafari Pro 6, I also select the "Set Time & Location" to pass that information automatically to my telescope.
//
//
//Serial pins
//ESP32:                                               Tx=(GPIO1/U0TxD) Output, Rx=(GPIO3/U0RxD) Input, VCC=3.3V
//MAX3232 - RS232 to TTL Serial Port Converter Module: Rx (Input) --> ESP32 Tx (Output),  Tx (Output) --> ESP32 Rx (Input), VCC --> ESP32 3.3V
//
// - Be careful! The AUX port of the telescope is NOT RS232 level compatible.
//   Do not connect the 3.3V TTL to RS232 level converter on this port!
//   Also, even if you'd make a 3.3V to 5V CMOS level converter for the AUX port,
//   using a 74HC125 CMOS buffer for example, the protocol of this port is different than what the hand controller is using on its RS232 PC port side.
// - One more thing to be careful about, is the fact that some TTL to RS232 level converters have their Tx/Rx pins marked backwards.
//   The Tx signal on the screenshot (XC3800-esp32-core-board-v2.png) is assumed to be an output on both the ESP32 and the TTL to RS232 converter,
//   and the Rx signal is an input on both as well.
//
//Settings used to upload the code:
//My board on Arduino IDS:          "ESP32 Dev Module" (XC3800 ESP32 Main Board with WiFi and Bluetooth)
//My board's serial port (Mac OS):  /dev/cu.usbserial-0001
//Upload speed:                     921600
//CPU Frequency:                    240MHz (WiFi/BT)
//Flash Frequency:                  80 MHz
//Flash Mode:                       QIO
//Flash Size:                       4MB (32Mbit)
//Partition Scheme:                 Default 4MB with SPIFFS (1.2MB App, 1.5MB SPIFFS)
//Core Debug Level:                 Verbose
//PSRAM:                            Disabled
//Arduino runs on:                  Core 1
//Events run on:                    Core 1
//
//
//To upload the code using th Arduino IDE, the moment you see the "Uploading" on the IDE, press the "Boot" button on the ESP32 and keep it pressed until you see the code to begin loading.
//
//
//After pressing the "EN" button on the ESP32 board (Serial Monitor at 115200):
//ets Jun  8 2016 00:22:57
//
//rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
//configsip: 0, SPIWP:0xee
//clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
//mode:DIO, clock div:1
//load:0x3fff0030,len:1240
//load:0x40078000,len:13012
//load:0x40080400,len:3648
//entry 0x400805f8
//[     0][V][WiFiServer.h:42] WiFiServer(): WiFiSer⸮f⸮⸮:WiFiServer(port=4030, ...)
//[     1][D][esp32-hal-cpu.c:211] setCpuFrequencyMhz(): PLL: 480 / 2 = 240 Mhz, APB: 80000000 Hz


const char* ssid = "SkyFi";                    // Replace with whatever name you want the ESP32 access point to present itself
const char* password = "aiuf324*hf234@!$Efdd"; // Replace with whatever you want as a password

WiFiServer server(4030); //This port is used by SkySafari Pro to communicate with the WiFi to Serial telescope adapter

void setup()
{
  Serial.begin(9600);//This is the RS232 baudrate for the Celestron NexStar 127SLT telescope. Modify it accordingly for your telescope.

  IPAddress local_IP(192,168,15,20);   // This is the IP address of the WiFi access pont that will be created, using the SSID above.
                                       // The client will normally get the next IP address (192.168.15.21) when it's connected, through DHCP
  IPAddress gateway(0,0,0,0);          // The access point will not be connected to the internet.
                                       // By adding a zero ip address here, it speeds up a lot the connection to the access point, because your device will not keep trying to the internet through it.
  IPAddress subnet(255,255,255,0);     // This is the subnet mask for the WiFi access point.

  // Connect to Wi-Fi network with SSID and password
  Serial.println("");
  Serial.print("Setting Access Point...");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();             // Start the WiFi Access Point
  Serial.print("Access Point IP address: ");
  Serial.println(IP);                         // This should be the "local_IP" that we declared above

  Serial.print("Access Point SSID: ");
  Serial.println(ssid);

  Serial.print("Access Point Password: ");
  Serial.println(password);

  server.begin();                            // Begin the TCP server on port 4030
}

void loop()
{
  char c;
  WiFiClient client = server.available(); // Listen for incoming TCP clients on port 4030

  if (client)                             // If a TCP client is connected
  {
    while (client.connected())
    {
      if (client.available())             // If the TCP client on port 4030 (iPad/PC) has sent any data
      {
        c = client.read();                // Read the data, character by character
        Serial.write(c);                  // Pass every character to the serial port
      }
      else                                // If the client is now waiting for a response from the telescope
      {
        while(Serial.available())         // If the telescope is sending data to the serial port
        {
          c=Serial.read();                // Read the data, character by character
          client.write(c);                // Pass every character to the TCP client on port 4030
        }
      }
    }
    client.stop();                       // Close the connection and go back to waiting for a new client
  }
}
