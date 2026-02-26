#pragma once

#include "esphome/core/component.h"
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"

#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif

#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif

#ifdef USE_ESP32

namespace esphome
{
    namespace spiderfarmer_ble
    {
        namespace espbt = esphome::esp32_ble_tracker;

        static const espbt::ESPBTUUID SPIDERFARMER_SERVICE_UUID = espbt::ESPBTUUID::from_raw(
            "000000ff-0000-1000-8000-00805f9b34fb");
        static const espbt::ESPBTUUID SPIDERFARMER_CHARACTERISTIC_UUID_NOTIFY = espbt::ESPBTUUID::from_raw(
            "0000ff01-0000-1000-8000-00805f9b34fb");
        static const espbt::ESPBTUUID SPIDERFARMER_CHARACTERISTIC_UUID_WRITE = espbt::ESPBTUUID::from_raw(
            "0000ff02-0000-1000-8000-00805f9b34fb");

        class SpiderfarmerBle : public esphome::ble_client::BLEClientNode, public PollingComponent
        {
        public:
            void setup() override;
            void update() override;
            void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                     esp_ble_gattc_cb_param_t* param) override;
            void dump_config() override;

#ifdef USE_BINARY_SENSOR
            void set_outlet_1_sensor(binary_sensor::BinarySensor* sensor) { this->outlet_1_sensor_ = sensor; }
            void set_outlet_2_sensor(binary_sensor::BinarySensor* sensor) { this->outlet_2_sensor_ = sensor; }
            void set_outlet_3_sensor(binary_sensor::BinarySensor* sensor) { this->outlet_3_sensor_ = sensor; }
            void set_outlet_4_sensor(binary_sensor::BinarySensor* sensor) { this->outlet_4_sensor_ = sensor; }
            void set_outlet_5_sensor(binary_sensor::BinarySensor* sensor) { this->outlet_5_sensor_ = sensor; }
            void set_outlet_6_sensor(binary_sensor::BinarySensor* sensor) { this->outlet_6_sensor_ = sensor; }
            void set_outlet_7_sensor(binary_sensor::BinarySensor* sensor) { this->outlet_7_sensor_ = sensor; }
            void set_outlet_8_sensor(binary_sensor::BinarySensor* sensor) { this->outlet_8_sensor_ = sensor; }
            void set_outlet_9_sensor(binary_sensor::BinarySensor* sensor) { this->outlet_9_sensor_ = sensor; }
            void set_outlet_10_sensor(binary_sensor::BinarySensor* sensor) { this->outlet_10_sensor_ = sensor; }

            void set_fan_natural_sensor(binary_sensor::BinarySensor* sensor) { this->fan_natural_sensor_ = sensor; }
#endif

#ifdef USE_SENSOR
            void set_sensor_soil_temp_sensor(sensor::Sensor* sensor) { this->sensor_soil_temp_sensor_ = sensor; }
            void set_sensor_soil_hum_sensor(sensor::Sensor* sensor) { this->sensor_soil_hum_sensor_ = sensor; }
            void set_sensor_soil_ec_sensor(sensor::Sensor* sensor) { this->sensor_soil_ec_sensor_ = sensor; }

            void set_sensor_temp_sensor(sensor::Sensor* sensor) { this->sensor_temp_sensor_ = sensor; }
            void set_sensor_hum_sensor(sensor::Sensor* sensor) { this->sensor_hum_sensor_ = sensor; }
            void set_sensor_vpd_sensor(sensor::Sensor* sensor) { this->sensor_vpd_sensor_ = sensor; }
            void set_sensor_ppfd_sensor(sensor::Sensor* sensor) { this->sensor_ppfd_sensor_ = sensor; }
            void set_sensor_co2_sensor(sensor::Sensor* sensor) { this->sensor_co2_sensor_ = sensor; }

            void set_light_level_sensor(sensor::Sensor* sensor) { this->light_level_sensor_ = sensor; }

            void set_blower_level_sensor(sensor::Sensor* sensor) { this->blower_level_sensor_ = sensor; }

            void set_fan_level_sensor(sensor::Sensor* sensor) { this->fan_level_sensor_ = sensor; }
#endif

        protected:
#ifdef USE_BINARY_SENSOR
            binary_sensor::BinarySensor* outlet_1_sensor_{nullptr};
            binary_sensor::BinarySensor* outlet_2_sensor_{nullptr};
            binary_sensor::BinarySensor* outlet_3_sensor_{nullptr};
            binary_sensor::BinarySensor* outlet_4_sensor_{nullptr};
            binary_sensor::BinarySensor* outlet_5_sensor_{nullptr};
            binary_sensor::BinarySensor* outlet_6_sensor_{nullptr};
            binary_sensor::BinarySensor* outlet_7_sensor_{nullptr};
            binary_sensor::BinarySensor* outlet_8_sensor_{nullptr};
            binary_sensor::BinarySensor* outlet_9_sensor_{nullptr};
            binary_sensor::BinarySensor* outlet_10_sensor_{nullptr};

            binary_sensor::BinarySensor* fan_natural_sensor_{nullptr};
#endif
#ifdef USE_SENSOR
            sensor::Sensor* sensor_soil_temp_sensor_{nullptr};
            sensor::Sensor* sensor_soil_hum_sensor_{nullptr};
            sensor::Sensor* sensor_soil_ec_sensor_{nullptr};

            sensor::Sensor* sensor_temp_sensor_{nullptr};
            sensor::Sensor* sensor_hum_sensor_{nullptr};
            sensor::Sensor* sensor_vpd_sensor_{nullptr};
            sensor::Sensor* sensor_ppfd_sensor_{nullptr};
            sensor::Sensor* sensor_co2_sensor_{nullptr};

            sensor::Sensor* light_level_sensor_{nullptr};

            sensor::Sensor* blower_level_sensor_{nullptr};

            sensor::Sensor* fan_level_sensor_{nullptr};
#endif
            uint16_t spiderfarmer_handle_;
            std::string receive_buffer_;
        };
    }
}

#endif
