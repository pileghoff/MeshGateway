# MeshGateway

This project was developed as my final thesis at DTU.
It is a Bluetooth Mesh gateway, for configuring and provisioning networks via MQTT.
It uses ESP-IDF and the ESP32-GATEWAY board from Olimex, which supports Ethernet, to avoid using BLE and Wifi at the same time.

## Building
Building takes place in docker, as the tool chain is quite bothersome to install.

1) build and start docker:
    `make docker`
2) Build the source code:
    `make build`
3) Flash to ESP32
    `make flash`
4) View serial output
    `make monitor`

