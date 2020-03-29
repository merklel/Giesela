# Gißela
**Automates watering the balcony plants**<br>
This repository holds the source code for the raspberry pi controlling the watering system and the code for the web interface to interact with the system. One can choose to start watering up to three times a day for a duration of 60 min in total. Manual enabling is possible via buttons or the web interface. Additionally, one can add a gmail-address and send a magic email to the system, which triggers the watering. By using the mail interface, one can easily control from outside the home network.

![Giselasystem](doc/overallSystem.jpg)

## Software

### Control Software
* C++ written, multithreading to control the waterpump and polling buttons
* listening for *magic email*
* keeps track of used water by integrating a fixed rate over time
* unix-sockets deal with the communication to the php backend of the webinterface.

### Webinterface to adjust Gißela
Simple HTML-based interface to control the gisela system. Runs also on the raspberry via apache2 webserver.
![Giselasystem](doc/gisela_webinterface.png)

## Hardware
* Raspberrypi zero gen1 + wifi stick
* 30l water canister
* camping water pump, 12V
* 12V Power supply
* 2 LEDs, 2 Buttons
* circuit to manage power supply for pi and pump
* wooden housing
* hose and dripper from hardware store

## More  Information
[More Information Here](https://github.com/merklel/Giesela/blob/master/doc/giesela.pdf)
