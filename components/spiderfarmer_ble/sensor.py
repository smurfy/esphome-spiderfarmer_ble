import esphome.codegen as cg
from esphome.components import ble_client, sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_TEMPERATURE,
    CONF_HUMIDITY,
    CONF_CO2,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_HUMIDITY,
    DEVICE_CLASS_CARBON_DIOXIDE,
    DEVICE_CLASS_PRESSURE,
    DEVICE_CLASS_EMPTY,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    UNIT_PARTS_PER_MILLION,
    UNIT_MILLISIEMENS_PER_CENTIMETER,
)
from . import CONF_SPIDERFARMER_ID, SpiderfarmerBle

DEPENDENCIES = ["spiderfarmer_ble"]

CONF_SOIL_TEMPERATURE = "soil_temperature"
CONF_SOIL_HUMIDITY = "soil_humidity"
CONF_SOIL_EC = "soil_ec"
CONF_VPD = "vpd"
CONF_PPFD = "ppfd"
CONF_LIGHT_LEVEL = "light_level"
CONF_BLOWER_LEVEL = "blower_level"
CONF_FAN_LEVEL = "fan_level"
UNIT_KILO_PASCAL = "kPa"
UNIT_UMOLM2S = "µmol/m²/s"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_SPIDERFARMER_ID): cv.use_id(SpiderfarmerBle),

            # SOIL
            cv.Optional(CONF_SOIL_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_SOIL_HUMIDITY): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_HUMIDITY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_SOIL_EC): sensor.sensor_schema(
                unit_of_measurement=UNIT_MILLISIEMENS_PER_CENTIMETER,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_HUMIDITY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),

            # TEMP, ETC.
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_HUMIDITY): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_HUMIDITY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_VPD): sensor.sensor_schema(
                unit_of_measurement=UNIT_KILO_PASCAL,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_PRESSURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_PPFD): sensor.sensor_schema(
                unit_of_measurement=UNIT_UMOLM2S,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_CO2): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_CARBON_DIOXIDE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),

            cv.Optional(CONF_LIGHT_LEVEL): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),

            cv.Optional(CONF_BLOWER_LEVEL): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),

            cv.Optional(CONF_FAN_LEVEL): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
)


async def to_code(config):
    var = await cg.get_variable(config[CONF_SPIDERFARMER_ID])

    if CONF_SOIL_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_SOIL_TEMPERATURE])
        cg.add(var.set_sensor_soil_temp_sensor(sens))
    if CONF_SOIL_HUMIDITY in config:
        sens = await sensor.new_sensor(config[CONF_SOIL_HUMIDITY])
        cg.add(var.set_sensor_soil_hum_sensor(sens))
    if CONF_SOIL_EC in config:
        sens = await sensor.new_sensor(config[CONF_SOIL_EC])
        cg.add(var.set_sensor_soil_ec_sensor(sens))

    if CONF_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(var.set_sensor_temp_sensor(sens))
    if CONF_HUMIDITY in config:
        sens = await sensor.new_sensor(config[CONF_HUMIDITY])
        cg.add(var.set_sensor_hum_sensor(sens))
    if CONF_VPD in config:
        sens = await sensor.new_sensor(config[CONF_VPD])
        cg.add(var.set_sensor_vpd_sensor(sens))
    if CONF_PPFD in config:
        sens = await sensor.new_sensor(config[CONF_PPFD])
        cg.add(var.set_sensor_ppfd_sensor(sens))
    if CONF_CO2 in config:
        sens = await sensor.new_sensor(config[CONF_CO2])
        cg.add(var.set_sensor_co2_sensor(sens))

    if CONF_LIGHT_LEVEL in config:
        sens = await sensor.new_sensor(config[CONF_LIGHT_LEVEL])
        cg.add(var.set_light_level_sensor(sens))

    if CONF_BLOWER_LEVEL in config:
        sens = await sensor.new_sensor(config[CONF_BLOWER_LEVEL])
        cg.add(var.set_blower_level_sensor(sens))

    if CONF_FAN_LEVEL in config:
        sens = await sensor.new_sensor(config[CONF_FAN_LEVEL])
        cg.add(var.set_fan_level_sensor(sens))
