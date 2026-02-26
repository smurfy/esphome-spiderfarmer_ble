# ESPHome spiderfarmer_ble component

This component allows you to easily get sensor data of your SpiderFarmer GGS system.

It uses the BLE/Bluetooth protocol to get those data. While it would also support setting data it currently only allows you to get data.

Based on research done by crO' here: https://github.com/cr0ssn0tice/Spider-Farmer-GGS-Controller-MQTT/

## Tested with the following GGS devices

Please be aware, at least 3.14 firmware on SF-GGS-CB adds encryption or some other kind of obfuscation to the data and it currently cant be decrypted by this component. 

### SF-GGS-CB (Controller)

Confirmed working, with Firmwares:
- 3.2

### SF-GGS-PS5 (Powerstrip 5 outlets)

Confirmed working, with Firmwares:
- 1.7

### SF-GGS-PS10 (Powerstrip 10 outlets)

Untested, but should work, just define more binary sensor outlets if you want to see their status.

### SF-GGS-LC (Light)

Connection works and system data (versions, name etc) are displayed, but data is quite different and not parsed, since the light is usually connected to a controller

## Configuration

You mainly need to specify the `mac_address` of your SpiderFarmer GGS.

````
esphome:
  name: "spider_sensor"

external_components:
  - source: github://smurfy/esphome-spiderfarmer_ble

esp32:
  board: esp32dev

logger:
  level: DEBUG

esp32_ble_tracker:

ble_client:
  - mac_address: "XX:XX:XX:XX:XX:XX"
    id: spidy_ble
    auto_connect: true 

spiderfarmer_ble:
  - id: spidy1
    ble_client_id: spidy_ble

sensor:
  - platform: spiderfarmer_ble
    spiderfarmer_id: spidy1
    soil_temperature:
       name: "Soil temperature"
    soil_humidity:
       name: "Soil humidity"
    soil_ec:
       name: "Soil ec"
    temperature:
       name: "Temperature"
    humidity:
       name: "Humidity"
    vpd:
       name: "VPD"
    ppfd:
       name: "PPFD"
    co2:
       name: "CO2"
    light_level:
       name: "Light level"
    blower_level:
       name: "Blower level"
    fan_level:
       name: "Fan level"

binary_sensor:
  - platform: spiderfarmer_ble
    spiderfarmer_id: spidy1
    outlet1_status:
      name: "Outlet 01"
    outlet2_status:
      name: "Outlet 02"
    outlet3_status:
      name: "Outlet 03"
    outlet4_status:
      name: "Outlet 04"
    outlet5_status:
      name: "Outlet 05"
    fan_natural:
      name: "Fan Natural"
      
text_sensor:
  - platform: spiderfarmer_ble
    spiderfarmer_id: spidy1
    device_id:
      name: "Device id"
    fw_version:
      name: "Firmware version"
    hw_version:
      name: "Hardware version"
````