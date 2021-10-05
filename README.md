# ESP32 WiFi Controller for the Celestron NexStar SLT telescope (works on many other telescopes that support SkyFi as well)
- Connect your iPad/PC to your Celestron NexStar SLT Telescope and control it with an astronomy app, like SkySafari Pro, K-Stars, or Stellarium
- I used an ESP32 dev module and a 3.3V TTL to RS232 level converter to connect it to the hand controller of my telescope.
- The code is pretty straight forward, and all it does is to create a WiFi access point, then start a TCP server listening on port 4030 and finally, pass the data between the connected TCP client (iPad/PC app) and the telescope's serial port.
- There is no protocol conversion performed, so this makes it compatible with any telescope mounts using this method to conenct over WiFi.
