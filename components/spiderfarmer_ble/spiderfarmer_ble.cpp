#include "spiderfarmer_ble.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/components/json/json_util.h"

#ifdef USE_ESP32

#include "psa/crypto.h"

namespace esphome
{
    namespace spiderfarmer_ble
    {
        static const char* const TAG = "spiderfarmer_ble";

        void data_data(std::string title, const std::string& str)
        {
            for (std::size_t i = 0; i < str.size(); i += 50) {
                ESP_LOGD(TAG, "%s: %s", title.c_str(), str.substr(i, 50).c_str());
            }
        }

        void data_json(JsonObject json)
        {
            std::string jsonStr = "";
            serializeJson(json, jsonStr);
            data_data("JSON", jsonStr);
        }

        uint16_t crc16(const uint8_t* data, size_t length) {
            uint16_t crc = 0xFFFF;

            for (size_t i = 0; i < length; ++i) {
                crc ^= data[i];
                for (int j = 0; j < 8; ++j) {
                    if (crc & 0x0001) {
                        crc = (crc >> 1) ^ 0xA001;
                    } else {
                        crc >>= 1;
                    }
                }
            }

            return crc;
        }

        psa_status_t encrypt_aes(
            const char *key, const char *iv,
            uint8_t *plaintext, size_t plaintext_len,
            uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_len)
        {
            memset(ciphertext, 0x00, ciphertext_size);

            psa_status_t status;
            psa_crypto_init();

            const size_t block_size = 16;

            // Calculate padded length
            size_t padded_len = plaintext_len;
            if (padded_len % block_size != 0) {
                padded_len += (block_size - (padded_len % block_size));
            }

            if (ciphertext_size < padded_len) {
                return PSA_ERROR_BUFFER_TOO_SMALL;
            }

            // Create padded buffer
            uint8_t padded[padded_len];
            memcpy(padded, plaintext, plaintext_len);
            memset(padded + plaintext_len, 0x00, padded_len - plaintext_len);

            psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
            psa_key_id_t key_id;

            psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
            psa_set_key_bits(&attr, 128);
            psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_ENCRYPT);
            psa_set_key_algorithm(&attr, PSA_ALG_CBC_NO_PADDING);

            status = psa_import_key(&attr, (const uint8_t*)key, 16, &key_id);

            ESP_LOGV(TAG, "IMPORT KEY: %d", status);
            if (status != PSA_SUCCESS) return status;

            psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
            size_t output_len = 0, total_len = 0;

            status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CBC_NO_PADDING);

            ESP_LOGV(TAG, "SETUP KEY: %d", status);
            if (status != PSA_SUCCESS) return status;

            status = psa_cipher_set_iv(&operation, (const uint8_t*)iv, 16);

            ESP_LOGV(TAG, "SETUP IV: %d", status);
            if (status != PSA_SUCCESS) return status;

            status = psa_cipher_update(&operation,
                                       padded, padded_len,
                                       ciphertext, ciphertext_size,
                                       &output_len);

            ESP_LOGV(TAG, "UPDATE WITH DATA: %d", status);
            if (status != PSA_SUCCESS) return status;

            total_len += output_len;

            status = psa_cipher_finish(&operation,
                                       ciphertext + total_len,
                                       ciphertext_size - total_len,
                                       &output_len);

            ESP_LOGV(TAG, "FIN: %d", status);
            if (status != PSA_SUCCESS) return status;

            total_len += output_len;
            *ciphertext_len = total_len;

            psa_destroy_key(key_id);

            return PSA_SUCCESS;
        }

        psa_status_t decrypt_aes(
            const char *key, const char *iv,
            uint8_t *ciphertext, size_t ciphertext_len,
            uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_len)
        {
            memset(plaintext, 0x00, plaintext_size);

            psa_status_t status;
            psa_crypto_init();

            psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
            psa_key_id_t key_id;

            psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
            psa_set_key_bits(&attr, 128);
            psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_DECRYPT);
            psa_set_key_algorithm(&attr, PSA_ALG_CBC_NO_PADDING);

            status = psa_import_key(&attr, (const uint8_t*)key, 16, &key_id);

            ESP_LOGV(TAG, "IMPORT KEY: %d", status);
            if (status != PSA_SUCCESS) return status;

            psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
            size_t output_len = 0, total_len = 0;

            status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CBC_NO_PADDING);

            ESP_LOGV(TAG, "SETUP KEY: %d", status);
            if (status != PSA_SUCCESS) return status;

            status = psa_cipher_set_iv(&operation, (const uint8_t*)iv, 16);

            ESP_LOGV(TAG, "SETUP IV: %d", status);
            if (status != PSA_SUCCESS) return status;

            status = psa_cipher_update(&operation,
                                       ciphertext, ciphertext_len,
                                       plaintext, plaintext_size,
                                       &output_len);

            ESP_LOGV(TAG, "UPDATE WITH DATA: %d", status);
            if (status != PSA_SUCCESS) return status;

            total_len += output_len;

            status = psa_cipher_finish(&operation,
                                       plaintext + total_len,
                                       plaintext_size - total_len,
                                       &output_len);

            ESP_LOGV(TAG, "FIN: %d", status);
            if (status != PSA_SUCCESS) return status;

            total_len += output_len;
            *plaintext_len = total_len;

            psa_destroy_key(key_id);

            return PSA_SUCCESS;
        }

        void SpiderfarmerBle::dump_config()
        {
            if (this->aes_key_ != nullptr && this->aes_iv_ != nullptr)
            {
                ESP_LOGCONFIG(TAG, "Spiderfarmer BLE Encryption is configured");
            }
            else
            {
                ESP_LOGCONFIG(TAG, "Spiderfarmer BLE Encryption is NOT configured");
            }

#ifdef USE_TEXT_SENSOR
            ESP_LOGCONFIG(TAG, "Spiderfarmer BLE (text sensor):");

            LOG_TEXT_SENSOR("  ", "Device id", this->deviceid_sensor_);
            LOG_TEXT_SENSOR("  ", "Firmware version", this->fwversion_sensor_);
            LOG_TEXT_SENSOR("  ", "Hardware version", this->hwversion_sensor_);
#endif

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
            if (this->last_received_data_.length() <= 0)
                return;

            auto json = json::parse_json(this->last_received_data_);
            // data_json(json.as<JsonObject>());

            std::string method = json["method"].as<std::string>();

            if (method == "getSysSta")
            {
                std::string deviceid = json["pid"].as<std::string>();
                JsonObject data = json["data"].as<JsonObject>();
                JsonObject sys = data["sys"].as<JsonObject>();

                std::string fwversion = sys["ver"].as<std::string>();
                std::string hwversion = sys["hwver"].as<std::string>();

                ESP_LOGV(TAG, "[%s] sys info: %s, %s, %s", this->parent_->address_str(),
                         deviceid.c_str(), fwversion.c_str(), hwversion.c_str());
#ifdef USE_TEXT_SENSOR
                if (this->deviceid_sensor_ != nullptr)
                    this->deviceid_sensor_->publish_state(deviceid);
                if (this->fwversion_sensor_ != nullptr)
                    this->fwversion_sensor_->publish_state(fwversion);
                if (this->hwversion_sensor_ != nullptr)
                    this->hwversion_sensor_->publish_state(hwversion);
#endif
            }
            else if (method == "getDevSta")
            {
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
                    data_json(outlet);

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
                    data_json(sensor);

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
                    data_json(light);

                    if (this->light_level_sensor_ != nullptr)
                    {
                        float level = light["level"].as<float>();
                        this->light_level_sensor_->publish_state(level);
                    }
                }

                if (!blower.isNull())
                {
                    data_json(blower);

                    if (this->blower_level_sensor_ != nullptr)
                    {
                        float level = blower["level"].as<float>();
                        this->blower_level_sensor_->publish_state(level);
                    }
                }

                if (!fan.isNull())
                {
                    data_json(fan);

                    if (this->fan_level_sensor_ != nullptr)
                    {
                        float level = fan["level"].as<float>();
                        this->fan_level_sensor_->publish_state(level);
                    }
                }
#endif
            }

            this->last_received_data_ = "";
        }

        void SpiderfarmerBle::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                                  esp_ble_gattc_cb_param_t* param)
        {
            switch (event)
            {
            case ESP_GATTC_ADV_DATA_EVT:
                {
                    ESP_LOGD(TAG, "ESP_GATTC_ADV_DATA_EVT triggered");

#ifdef USE_TEXT_SENSOR
                    // if (this->friendly_name_sensor_ != nullptr)
                    //     this->friendly_name_sensor_->publish_state((char*)param->read.value, param->read.value_len);
#endif
                    break;
                }
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
                        break;
                    }

                    auto* chr2 = this->parent_->get_characteristic(SPIDERFARMER_SERVICE_UUID,
                                                                   SPIDERFARMER_CHARACTERISTIC_UUID_WRITE);
                    if (chr2 == nullptr)
                    {
                        ESP_LOGE(TAG, "[%s] No spiderfarmer write service found at device, not an Spiderfarmer..?",
                                 this->parent_->address_str());
                        break;
                    }

                    this->spiderfarmer_notify_handle_ = chr->handle;
                    this->spiderfarmer_write_handle_ = chr2->handle;
                    break;
                }
            case ESP_GATTC_REG_FOR_NOTIFY_EVT:
                {
                    this->node_state = espbt::ClientState::ESTABLISHED;
                    this->receive_buffer_ = "";
                    this->update();
                    break;
                }
            case ESP_GATTC_NOTIFY_EVT:
                {
                    if (param->notify.handle == this->spiderfarmer_notify_handle_)
                    {
                        //ESP_LOGV(TAG, "[%s] raw data %d: %s", this->parent_->address_str(), param->notify.value_len, format_hex(param->notify.value, param->notify.value_len).c_str());
                        if (param->notify.value_len > 22)
                        {
                            // has a valid header
                            if (param->notify.value[0] == 0xAA && param->notify.value[1] == 0xAA
                                && param->notify.value[2] == 0x00 && param->notify.value[3] == 0x03)
                            {
                                this->parse_data(param->notify.value, param->notify.value_len);
                            }
                            // looks like direct json without header
                            else if (param->notify.value[0] == 0x7B && param->notify.value[1] == 0x22 && param->notify.
                                value[2] == 0x6d)
                            {
                                data_data("JSON DATA", format_hex(param->notify.value, param->notify.value_len).c_str());

                                this->receive_buffer_encrypted_ = false;
                                this->add_to_buffer(param->notify.value, param->notify.value_len);
                                this->handle_data();
                                this->update();
                            }
                        }
                    }
                    break;
                }
            default:
                break;
            }
        }

        void SpiderfarmerBle::add_to_buffer(uint8_t *data, size_t data_len)
        {
            for (int i = 0; i < data_len; i++)
            {
                char c = static_cast<char>(data[i]);
                this->receive_buffer_ += c;
            }
        }

        void SpiderfarmerBle::parse_data(uint8_t *data, size_t data_len)
        {
            // Extract payload size
            uint16_t payload_size = ((uint16_t)data[4] << 8) |
                    ((uint16_t)data[5]);

            if (payload_size + 8 == data_len)
            {
                ESP_LOGV(TAG, "[%s] looks like good data. got %d", this->parent_->address_str(), payload_size);

                // Unencrypted
                // aaaa0003 019e 0001 99e7 00000246 00000000 0190 7b226d65746...
                // aaaa0003 00c5 0001 438f 00000247 00000190 00b7 70646174654...
                // Encrypted
                // aaaa0003 00de 0002 046d 000000d0 00000000 00d0 8faabc89655...

                uint16_t total_crc = ((uint16_t)data[8] << 8) |
                    ((uint16_t)data[9]);
                uint32_t total_size = ((uint32_t)data[10] << 24) |
                    ((uint32_t)data[11] << 16) |
                    ((uint32_t)data[12] << 8) |
                    ((uint32_t)data[13]);
                uint32_t block_offset = ((uint32_t)data[14] << 24) |
                    ((uint32_t)data[15] << 16) |
                    ((uint32_t)data[16] << 8) |
                    ((uint32_t)data[17]);
                uint16_t block_size = ((uint16_t)data[18] << 8) |
                    ((uint16_t)data[19]);

                uint16_t package_crc = ((uint16_t)data[20 + block_size] << 8) |
                    ((uint16_t)data[20 + block_size + 1]);

                uint8_t msgtype[2];
                std::memcpy(msgtype, data + 6, 2);

                uint16_t calculated_crc = crc16(data, 20 + block_size);

                ESP_LOGD(TAG, "HEADER: %s", format_hex(data, 20).c_str());
                ESP_LOGD(TAG, "Decoded: MSG: %s TOTAL: %d OFF: %d SIZE: %d CRC: %d vs %d", format_hex(msgtype, 2).c_str(), total_size, block_offset, block_size, calculated_crc, package_crc);

                if (package_crc != calculated_crc)
                {
                    ESP_LOGW(TAG, "Received invalid data, checksum do not match of the package received");
                }

                uint8_t *block = (uint8_t*)malloc(block_size);
                std::memcpy(block, data + 20, block_size);

                if (msgtype[0] == 0x00 && msgtype[1] == 0x01)
                {
                    this->receive_buffer_encrypted_ = false;
                }
                else if (msgtype[0] == 0x00 && msgtype[1] == 0x02)
                {
                    this->receive_buffer_encrypted_ = true;
                }
                else
                {
                    // NO IDEA
                    return;
                }

                data_data("BLOCK DATA", format_hex(block, block_size).c_str());

                // If block_offset is zero it means clean new package arrived, so we clear the receive_buffer
                if (block_offset == 0)
                    this->receive_buffer_ = "";

                this->add_to_buffer(block, block_size);

                // Receive buffer should now contain the whole data, so we can begin parsing
                if (this->receive_buffer_.size() >= total_size)
                {
                    uint16_t calculated_total_crc = crc16((uint8_t*)this->receive_buffer_.data(), this->receive_buffer_.size());

                    if (calculated_total_crc != total_crc)
                    {
                        ESP_LOGW(TAG, "Received invalid data, checksum do not match of the package received");
                    }

                    this->handle_data();
                    this->update();
                }
            }
        }

        void SpiderfarmerBle::handle_data()
        {
            size_t data_size = this->receive_buffer_.size();
            size_t data_len = data_size;
            uint8_t *data = (uint8_t*)malloc(data_size);
            memcpy(data, this->receive_buffer_.data(), data_size);

            if (this->receive_buffer_encrypted_)
            {
                uint8_t *data_encrypted = (uint8_t*)malloc(data_size);
                std::memcpy(data_encrypted, data, data_size);

                if (this->aes_key_ != nullptr && this->aes_iv_ != nullptr)
                {
                    data_data("ENCRYPTED DATA", format_hex(data_encrypted, data_size).c_str());

                    psa_status_t status = decrypt_aes(
                        this->aes_key_, this->aes_iv_,
                        data_encrypted, data_size,
                        data, data_size,
                        &data_len
                    );

                    if (status != PSA_SUCCESS) {
                        ESP_LOGW(TAG, "ERR DECRYPTING: %d", status);
                    }
                }
                else
                {
                    ESP_LOGW(TAG, "Data cant be decrypted, no key/iv configured");

                }
            }

            data_data("PLAIN DATA", format_hex(data, data_len).c_str());

            for (size_t i = 0; i < data_len; i++) {
                if (data[i] == '{' || data[i] == '[') {

                    JsonDocument doc;
                    DeserializationError err = deserializeJson(
                      doc,
                      data + i,
                      data_len - i
                    );

                    if (!err) {
                        JsonVariant v = doc.as<JsonVariant>();
                        size_t consumed = measureJson(v);

                        // ESP_LOGD(TAG, "[%s] Data: Len: %d and json is: %d", this->parent_->address_str(), data_len, consumed);

                        this->receive_buffer_.erase(this->receive_buffer_.begin(), this->receive_buffer_.begin() + i + consumed);

                        serializeJson(doc, this->last_received_data_);
                        break;
                    }
                }
            }
        }

        void SpiderfarmerBle::send_data(JsonObject json, bool encrypt)
        {
            // TODO add check for size, it seems spider farmer HW only sends 400 bytes at once
            // TODO support for sending unencrypted too
            data_json(json);

            std::string json_str = "";
            serializeJson(json, json_str);

            const char *json_str_data = json_str.c_str();
            size_t json_str_len = strlen(json_str_data);

            uint8_t *ciphertext = (uint8_t*)malloc(json_str_len + 16);
            size_t ciphertext_len;

            psa_status_t status = encrypt_aes(
                this->aes_key_, this->aes_iv_,
                (uint8_t*)json_str_data, json_str_len,
                ciphertext, json_str_len + 16,
                &ciphertext_len
            );

            if (status != PSA_SUCCESS) {
                ESP_LOGW(TAG, "ERR ENCRYPTING: %d", status);
            }

            data_data("SEND ENCRYPTED DATA", format_hex(ciphertext, ciphertext_len).c_str());

            uint16_t calculated_total_crc = crc16(ciphertext, ciphertext_len);
            uint8_t *data = (uint8_t*)malloc(ciphertext_len + 22);

            data[0] = 0xaa;
            data[1] = 0xaa;
            data[2] = 0x00;
            data[3] = 0x03;
            data[4] = ((ciphertext_len + 14) >> 8) & 0xFF; // TOTAL PACKAGE LEN
            data[5] = (ciphertext_len + 14) & 0xFF;        // TOTAL PACKAGE LEN
            data[6] = 0x00;
            data[7] = 0x02; // 01 = plain, 02 = encrypted
            data[8] = (calculated_total_crc >> 8) & 0xFF; // TOTAL PACKAGE CRC
            data[9] = calculated_total_crc & 0xFF;        // TOTAL PACKAGE CRC
            data[10] = (ciphertext_len >> 24) & 0xFF; // TOTAL LEN
            data[11] = (ciphertext_len >> 16) & 0xFF; // TOTAL LEN
            data[12] = (ciphertext_len >> 8) & 0xFF;  // TOTAL LEN
            data[13] = ciphertext_len & 0xFF;         // TOTAL LEN
            data[14] = 0x00; // BLOCK OFFSET
            data[15] = 0x00; // BLOCK OFFSET
            data[16] = 0x00; // BLOCK OFFSET
            data[17] = 0x00; // BLOCK OFFSET
            data[18] = (ciphertext_len >> 8) & 0xFF; // BLOCK SIZE
            data[19] = ciphertext_len & 0xFF;        // BLOCK SIZE
            memcpy(data + 20, ciphertext, ciphertext_len); // BLOCK DATA
            uint16_t calculated_crc = crc16(data, 20 + ciphertext_len);
            data[20 + ciphertext_len] = (calculated_crc >> 8) & 0xFF;; // CRC of everything
            data[21 + ciphertext_len] = calculated_crc & 0xFF; // CRC of everything

            data_data("SEND DATA", format_hex(data, ciphertext_len + 22).c_str());

            auto status2 = esp_ble_gattc_write_char(this->parent_->get_gattc_if(), this->parent_->get_conn_id(),
                                                   this->spiderfarmer_write_handle_, ciphertext_len + 22, data,
                                                   ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);

            if (status2) {
                ESP_LOGW(TAG, "esp_ble_gattc_write_char error, status=%d", status2);
                return;
            }

            ESP_LOGD(TAG, "DATA SENT!");

            // // FIXME DEBUG
            // ESP_LOGD(TAG, "DEBUG SENT BEGIN");
            // this->parse_data(data, ciphertext_len + 22);
            // ESP_LOGD(TAG, "DEBUG SENT END");
        }
    }
}

#endif
