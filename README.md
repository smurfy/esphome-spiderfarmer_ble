# ESPHome spiderfarmer_ble component

This component allows you to easily get sensor data of your SpiderFarmer GGS system.

It uses the BLE/Bluetooth protocol to get those data. While it would also support setting data it currently only allows you to get data.

Based on research done by crO' here: https://github.com/cr0ssn0tice/Spider-Farmer-GGS-Controller-MQTT/

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
       name: "Light"
    blower_level:
       name: "Blower"
    fan_level:
       name: "Fan"

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
````