#include "spiderfarmer_ble.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/components/json/json_util.h"

#ifdef USE_ESP32

namespace esphome
{
    namespace spiderfarmer_ble
    {
        static const char* const TAG = "spiderfarmer_ble";

        void dumpJson(JsonObject json)
        {
            std::string jsonStr = "";
            serializeJson(json, jsonStr);
            ESP_LOGV(TAG, "Data: %s", jsonStr.c_str());
        }

        void SpiderfarmerBle::dump_config()
        {
#ifdef USE_BINARY_SENSOR
            ESP_LOGCONFIG(TAG, "Spiderfarmer BLE (binary sensor):");

            LOG_BINARY_SENSOR("  ", "Outlet 01", this->outlet_1_sensor_);
            LOG_BINARY_SENSOR("  ", "Outlet 02", this->outlet_2_sensor_);
            LOG_BINARY_SENSOR("  ", "Outlet 03", this->outlet_3_sensor_);
            LOG_BINARY_SENSOR("  ", "Outlet 04", this->outlet_4_sensor_);
            LOG_BINARY_SENSOR("  ", "Outlet 05", this->outlet_5_sensor_);
            LOG_BINARY_SENSOR("  ", "Outlet 06", this->outlet_6_sensor_);
            LOG_BINARY_SENSOR("  ", "Outlet 07", this->outlet_7_sensor_);
            LOG_BINARY_SENSOR("  ", "Outlet 08", this->outlet_8_sensor_);
            LOG_BINARY_SENSOR("  ", "Outlet 09", this->outlet_9_sensor_);
            LOG_BINARY_SENSOR("  ", "Outlet 10", this->outlet_10_sensor_);

            LOG_BINARY_SENSOR("  ", "Fan natural", this->fan_natural_sensor_);
#endif

#ifdef USE_SENSOR
            ESP_LOGCONFIG(TAG, "Spiderfarmer BLE (sensor):");

            LOG_SENSOR("  ", "Soil temperature", this->sensor_soil_temp_sensor_);
            LOG_SENSOR("  ", "Soil humidity", this->sensor_soil_hum_sensor_);
            LOG_SENSOR("  ", "Soil ec", this->sensor_soil_ec_sensor_);

            LOG_SENSOR("  ", "Temperature", this->sensor_temp_sensor_);
            LOG_SENSOR("  ", "Humidity", this->sensor_hum_sensor_);
            LOG_SENSOR("  ", "VPD", this->sensor_vpd_sensor_);
            LOG_SENSOR("  ", "PPFD", this->sensor_ppfd_sensor_);
            LOG_SENSOR("  ", "CO2", this->sensor_co2_sensor_);

            LOG_SENSOR("  ", "Light level", this->light_level_sensor_);
            LOG_SENSOR("  ", "Blower level", this->blower_level_sensor_);
            LOG_SENSOR("  ", "Fan level", this->fan_level_sensor_);
#endif
        }

        void SpiderfarmerBle::setup()
        {
        }

        void SpiderfarmerBle::update()
        {
        }

        void SpiderfarmerBle::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                                  esp_ble_gattc_cb_param_t* param)
        {
            switch (event)
            {
            case ESP_GATTC_OPEN_EVT:
                {
                    if (param->open.status == ESP_GATT_OK)
                    {
                        ESP_LOGI(TAG, "[%s] connection open", this->parent_->address_str());
                    }
                    break;
                }
            case ESP_GATTC_CONNECT_EVT:
                {
                    if (std::memcmp(param->connect.remote_bda, this->parent_->get_remote_bda(), 6) != 0)
                        return;
                    break;
                }
            case ESP_GATTC_DISCONNECT_EVT:
                {
                    this->node_state = espbt::ClientState::IDLE;
                    this->receive_buffer_ = "";
#ifdef USE_BINARY_SENSOR
                    if (this->outlet_1_sensor_ != nullptr)
                        this->outlet_1_sensor_->publish_state(NAN);
                    if (this->outlet_2_sensor_ != nullptr)
                        this->outlet_2_sensor_->publish_state(NAN);
                    if (this->outlet_3_sensor_ != nullptr)
                        this->outlet_3_sensor_->publish_state(NAN);
                    if (this->outlet_4_sensor_ != nullptr)
                        this->outlet_4_sensor_->publish_state(NAN);
                    if (this->outlet_5_sensor_ != nullptr)
                        this->outlet_5_sensor_->publish_state(NAN);
                    if (this->outlet_6_sensor_ != nullptr)
                        this->outlet_6_sensor_->publish_state(NAN);
                    if (this->outlet_7_sensor_ != nullptr)
                        this->outlet_7_sensor_->publish_state(NAN);
                    if (this->outlet_8_sensor_ != nullptr)
                        this->outlet_8_sensor_->publish_state(NAN);
                    if (this->outlet_9_sensor_ != nullptr)
                        this->outlet_9_sensor_->publish_state(NAN);
                    if (this->outlet_10_sensor_ != nullptr)
                        this->outlet_10_sensor_->publish_state(NAN);

                    if (this->fan_natural_sensor_ != nullptr)
                        this->fan_natural_sensor_->publish_state(NAN);
#endif
#ifdef USE_SENSOR
                    if (this->sensor_soil_temp_sensor_ != nullptr)
                        this->sensor_soil_temp_sensor_->publish_state(NAN);
                    if (this->sensor_soil_hum_sensor_ != nullptr)
                        this->sensor_soil_hum_sensor_->publish_state(NAN);
                    if (this->sensor_soil_ec_sensor_ != nullptr)
                        this->sensor_soil_ec_sensor_->publish_state(NAN);

                    if (this->sensor_temp_sensor_ != nullptr)
                        this->sensor_temp_sensor_->publish_state(NAN);
                    if (this->sensor_hum_sensor_ != nullptr)
                        this->sensor_hum_sensor_->publish_state(NAN);
                    if (this->sensor_vpd_sensor_ != nullptr)
                        this->sensor_vpd_sensor_->publish_state(NAN);
                    if (this->sensor_ppfd_sensor_ != nullptr)
                        this->sensor_ppfd_sensor_->publish_state(NAN);
                    if (this->sensor_co2_sensor_ != nullptr)
                        this->sensor_co2_sensor_->publish_state(NAN);

                    if (this->light_level_sensor_ != nullptr)
                        this->light_level_sensor_->publish_state(NAN);

                    if (this->blower_level_sensor_ != nullptr)
                        this->blower_level_sensor_->publish_state(NAN);

                    if (this->fan_level_sensor_ != nullptr)
                        this->fan_level_sensor_->publish_state(NAN);
#endif
                    break;
                }
            case ESP_GATTC_SEARCH_CMPL_EVT:
                {
                    auto* chr = this->parent_->get_characteristic(SPIDERFARMER_SERVICE_UUID,
                                                                  SPIDERFARMER_CHARACTERISTIC_UUID_NOTIFY);
                    if (chr == nullptr)
                    {
                        ESP_LOGE(TAG, "[%s] No spiderfarmer service found at device, not an Spiderfarmer..?",
                                 this->parent_->address_str());
                        break;
                    }
                    auto status = esp_ble_gattc_register_for_notify(this->parent_->get_gattc_if(),
                                                                    this->parent_->get_remote_bda(),
                                                                    chr->handle);
                    if (status)
                    {
                        ESP_LOGW(TAG, "esp_ble_gattc_register_for_notify failed, status=%d", status);
                    }
                    this->spiderfarmer_handle_ = chr->handle;
                    break;
                }
            case ESP_GATTC_REG_FOR_NOTIFY_EVT:
                {
                    this->node_state = espbt::ClientState::ESTABLISHED;
                    this->update();
                    break;
                }
            case ESP_GATTC_NOTIFY_EVT:
                {
                    if (param->notify.handle == this->spiderfarmer_handle_)
                    {
                        //ESP_LOGV(TAG, "[%s] raw data %d: %s", this->parent_->address_str(), param->notify.value_len, format_hex(param->notify.value, param->notify.value_len).c_str());
                        if (param->notify.value_len > 22)
                        {
                            // First header
                            if (param->notify.value[0] == 0xAA && param->notify.value[1] == 0xAA && param->notify.value[
                                2] == 0x00 && param->notify.value[3] == 0x03)
                            {
                                // Extract payload size
                                uint16_t data_size = (static_cast<uint16_t>(param->notify.value[4]) << 8) | param->
                                    notify.value[5];
                                if (data_size + 8 == param->notify.value_len)
                                {
                                    //ESP_LOGV(TAG, "[%s] looks like good data. got %d", this->parent_->address_str(), data_size);
                                    for (int i = 12; i < data_size - 2; i++)
                                    {
                                        char c = (char)param->notify.value[i + 8];
                                        // Filtering out some crap
                                        if (c >= 32 && c <= 126)
                                        {
                                            this->receive_buffer_ += c;
                                        }
                                    }

                                    if (this->receive_buffer_.find("method\"") != std::string::npos && this->
                                        receive_buffer_.find("}}}") != std::string::npos)
                                    {
                                        //ESP_LOGV(TAG, "[%s] Data: %s", this->parent_->address_str(), this->receive_buffer_.c_str());
                                        auto json = json::parse_json(this->receive_buffer_);
                                        JsonObject data = json["data"].as<JsonObject>();

                                        // These will come via method: getDevSta
                                        JsonObject outlet = data["outlet"].as<JsonObject>();
                                        // {"psmode":1,"O1":{"on":0,"mode":1},"O2":{"on":1,"mode":0},"O3":{"on":0,"mode":0},"O4":{"on":1,"mode":1},"O5":{"on":0,"mode":0}}
                                        JsonObject sensor = data["sensor"].as<JsonObject>();
                                        // {"tempUnit":0,"temp":27.8,"humi":46.8,"vpd":1.99,"isDaySensor":1,"ppfd":737,"co2":2146,"tempSoil":21.9,"humiSoil":26.3,"ECSoil":0.5,"isDayEnvTarget":0}
                                        JsonObject light = data["light"].as<JsonObject>(); // {"mode":1,"level":100}
                                        JsonObject blower = data["blower"].as<JsonObject>();
                                        // {"mode":0,"ctlMode":1,"level":33,"aemodel":0,"adcManual":0,"freq":71}
                                        JsonObject fan = data["fan"].as<JsonObject>();
                                        // {"mode":0,"level":0,"shakeLevel":0,"shakeAngle":0,"natural":0,"pwm":0,"freq":0}

#ifdef USE_BINARY_SENSOR
                                        if (!outlet.isNull())
                                        {
                                            dumpJson(outlet);

                                            if (this->outlet_1_sensor_ != nullptr)
                                            {
                                                JsonObject outletdata = outlet["O1"].as<JsonObject>();
                                                int status = outletdata["on"].as<int>();
                                                this->outlet_1_sensor_->publish_state(status > 0);
                                            }
                                            if (this->outlet_2_sensor_ != nullptr)
                                            {
                                                JsonObject outletdata = outlet["O2"].as<JsonObject>();
                                                int status = outletdata["on"].as<int>();
                                                this->outlet_2_sensor_->publish_state(status > 0);
                                            }
                                            if (this->outlet_3_sensor_ != nullptr)
                                            {
                                                JsonObject outletdata = outlet["O3"].as<JsonObject>();
                                                int status = outletdata["on"].as<int>();
                                                this->outlet_3_sensor_->publish_state(status > 0);
                                            }
                                            if (this->outlet_4_sensor_ != nullptr)
                                            {
                                                JsonObject outletdata = outlet["O4"].as<JsonObject>();
                                                int status = outletdata["on"].as<int>();
                                                this->outlet_4_sensor_->publish_state(status > 0);
                                            }
                                            if (this->outlet_5_sensor_ != nullptr)
                                            {
                                                JsonObject outletdata = outlet["O5"].as<JsonObject>();
                                                int status = outletdata["on"].as<int>();
                                                this->outlet_5_sensor_->publish_state(status > 0);
                                            }
                                            if (this->outlet_6_sensor_ != nullptr)
                                            {
                                                JsonObject outletdata = outlet["O6"].as<JsonObject>();
                                                int status = outletdata["on"].as<int>();
                                                this->outlet_6_sensor_->publish_state(status > 0);
                                            }
                                            if (this->outlet_7_sensor_ != nullptr)
                                            {
                                                JsonObject outletdata = outlet["O7"].as<JsonObject>();
                                                int status = outletdata["on"].as<int>();
                                                this->outlet_7_sensor_->publish_state(status > 0);
                                            }
                                            if (this->outlet_8_sensor_ != nullptr)
                                            {
                                                JsonObject outletdata = outlet["O8"].as<JsonObject>();
                                                int status = outletdata["on"].as<int>();
                                                this->outlet_8_sensor_->publish_state(status > 0);
                                            }
                                            if (this->outlet_9_sensor_ != nullptr)
                                            {
                                                JsonObject outletdata = outlet["O9"].as<JsonObject>();
                                                int status = outletdata["on"].as<int>();
                                                this->outlet_9_sensor_->publish_state(status > 0);
                                            }
                                            if (this->outlet_10_sensor_ != nullptr)
                                            {
                                                JsonObject outletdata = outlet["O10"].as<JsonObject>();
                                                int status = outletdata["on"].as<int>();
                                                this->outlet_10_sensor_->publish_state(status > 0);
                                            }
                                        }

                                        if (!sensor.isNull())
                                        {
                                            if (!fan.isNull())
                                            {
                                                if (this->fan_natural_sensor_ != nullptr)
                                                {
                                                    int natural = fan["natural"].as<int>();
                                                    this->fan_natural_sensor_->publish_state(natural > 0);
                                                }
                                            }
                                        }
#endif

#ifdef USE_SENSOR
                                        if (!sensor.isNull())
                                        {
                                            dumpJson(sensor);

                                            if (this->sensor_soil_temp_sensor_ != nullptr)
                                            {
                                                float temperature = sensor["tempSoil"].as<float>();
                                                this->sensor_soil_temp_sensor_->publish_state(temperature);
                                            }
                                            if (this->sensor_soil_hum_sensor_ != nullptr)
                                            {
                                                float humidity = sensor["humiSoil"].as<float>();
                                                this->sensor_soil_hum_sensor_->publish_state(humidity);
                                            }
                                            if (this->sensor_soil_ec_sensor_ != nullptr)
                                            {
                                                float ec = sensor["ECSoil"].as<float>();
                                                this->sensor_soil_ec_sensor_->publish_state(ec);
                                            }
                                            if (this->sensor_temp_sensor_ != nullptr)
                                            {
                                                float temperature = sensor["temp"].as<float>();
                                                this->sensor_temp_sensor_->publish_state(temperature);
                                            }
                                            if (this->sensor_hum_sensor_ != nullptr)
                                            {
                                                float humidity = sensor["humi"].as<float>();
                                                this->sensor_hum_sensor_->publish_state(humidity);
                                            }
                                            if (this->sensor_vpd_sensor_ != nullptr)
                                            {
                                                float vpd = sensor["vpd"].as<float>();
                                                this->sensor_vpd_sensor_->publish_state(vpd);
                                            }
                                            if (this->sensor_ppfd_sensor_ != nullptr)
                                            {
                                                float ppfd = sensor["ppfd"].as<float>();
                                                this->sensor_ppfd_sensor_->publish_state(ppfd);
                                            }
                                            if (this->sensor_co2_sensor_ != nullptr)
                                            {
                                                float co2 = sensor["co2"].as<float>();
                                                this->sensor_co2_sensor_->publish_state(co2);
                                            }
                                        }

                                        if (!light.isNull())
                                        {
                                            dumpJson(light);

                                            if (this->light_level_sensor_ != nullptr)
                                            {
                                                float level = light["level"].as<float>();
                                                this->light_level_sensor_->publish_state(level);
                                            }
                                        }

                                        if (!blower.isNull())
                                        {
                                            dumpJson(blower);

                                            if (this->blower_level_sensor_ != nullptr)
                                            {
                                                float level = blower["level"].as<float>();
                                                this->blower_level_sensor_->publish_state(level);
                                            }
                                        }

                                        if (!fan.isNull())
                                        {
                                            dumpJson(fan);

                                            if (this->fan_level_sensor_ != nullptr)
                                            {
                                                float level = fan["level"].as<float>();
                                                this->fan_level_sensor_->publish_state(level);
                                            }
                                        }
#endif
                                        this->receive_buffer_ = "";
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
            default:
                break;
            }
        }
    }
}

#endif
