## IOT NMKR 

<img src="https://github.com/elRaulito/IoT-NMKR-integration-Open-Source-/tree/main/images/1.png" width="400">
This project was funded by Catalyst Fund 10 and integrates minting of NFTs on ESP32 embedded device thanks to NMKR studio APIs.

# Bill of Materials

![BOM](https://github.com/elRaulito/IoT-NMKR-integration-Open-Source-/tree/main/images/2.png?raw=true)

In order to run this project you will need as hardware:

- ESP32 CAM
- FTDI232 as programmer

# Build an flash

After installing ESP-IDF 4.0 (**Some libraries don't work with 4.1 version**) and the esp-camera library (it is not already included in the components of esp-idf) go into the AIcam folder open esp-idf and run the command
```
idf.py all
```

Now the project should be built and you are ready to flash it.
Connect the esp32-cam as follows with FTDI232
| ESP32-CAM GPIO| FTDI232 |
| ------------- | ------------- |
| 3V3           | VCC           |
| GND           | GND           |
| V0R           | TX           |
| V0T           | RX           |
| IO0 with GND   |  --          |

![connection](https://github.com/elRaulito/IoT-NMKR-integration-Open-Source-/tree/main/images/3.png?raw=true)

Press the reset button on esp32-cam and run the command 

```
idf.py flash

```
# NMKR STUDIO CONFIGURATION

![NMKR](https://github.com/elRaulito/IoT-NMKR-integration-Open-Source-/tree/main/images/4.png?raw=true)


In order to run the Camera and mint NFTs we will need to setup an account on NMKR studio:



# Running the program

To use the cam the connection may be just the power supply, therefore like the following
| ESP32-CAM GPIO| FTDI232 |
| ------------- | ------------- |
| 3V3           | VCC           |
| GND           | GND           |


Press the reset button and the camera will start!

# Configuration

When the esp32-cam turns on it will generate its own wifi, the name is **CameraIoTraffic** and the password is **12345678**, after connecting
open a browser and go to the page http://192.168.1.1 
This page will appear:

![dashboard](https://github.com/elRaulito/Iotraffic-hackaton/blob/main/AIcam/AI-dash/dash.PNG?raw=true)

Before pressing Activate camera let's configure all the parameters (**parameters are stored in Not volatile storage of ESP32 therefore you need to set everything only the first time**), 
like the wifi

![wifi](https://github.com/elRaulito/Iotraffic-hackaton/blob/main/AIcam/AI-dash/wifi.PNG?raw=true)

the address and the port of Keepy

![keepy](https://github.com/elRaulito/Iotraffic-hackaton/blob/main/AIcam/AI-dash/keepy.PNG?raw=true)

the address and the port of myPersonalAI

![AI](https://github.com/elRaulito/Iotraffic-hackaton/blob/main/AIcam/AI-dash/AIport.PNG?raw=true)

the device ID in the whitelist in config.json of streams-http-gateway

![device](https://github.com/elRaulito/Iotraffic-hackaton/blob/main/AIcam/AI-dash/device.PNG?raw=true)

The objects that i want to detect and pubblish on the tangle

![detection](https://github.com/elRaulito/Iotraffic-hackaton/blob/main/AIcam/AI-dash/detection.PNG?raw=true)

Now just press **activate camera** and your camera will start to pubblish detected objects on the Tangle.
