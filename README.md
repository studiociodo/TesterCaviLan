# TesterCaviLan
During IT deployment/mantaining/troubleshooting most time you have to figure out what IP is on that VLAN or search for cable and go around with a notebook is not easy, So I create this project for create a troubleshooting tool that fits in the palm of your hand!
First go on Physical layer and lights up the switch and search for DHCP, if it found view on screen IP Address that it take,Subnet,Gateway and First DNS.
After that go online directly to the IP address of a website to obtain your WAN IP, Your ISP and Geographic position.
After that create a DNS Query for check if resolver was correct and Internet work flawlessly.
At the end of this OR If any of this test fail, start to disable/enable LAN port with 6 seconds of interval, so you can go in front of a switch for figure out led light up and down!

![](https://github.com/studiociodo/TesterCaviLan/blob/main/sample_demo.gif)

Here is a schematic and Arduino code, happy deploy!

![](https://github.com/studiociodo/TesterCaviLan/blob/main/schematic.png)

Part Listing:
1 X Arduino Nano
1 X W5100 Mini Ethernet Module
1 X OLED Display 128x64 0.96'' SSD1306 I2C
1 X TP4056 charger module
1 X LiIon Battery
