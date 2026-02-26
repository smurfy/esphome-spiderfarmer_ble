import esphome.codegen as cg
from esphome.components import ble_client, binary_sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_POWER
)

from . import CONF_SPIDERFARMER_ID, SpiderfarmerBle

DEPENDENCIES = ["spiderfarmer_ble"]

CONF_OUTLET1_STATUS = "outlet1_status"
CONF_OUTLET2_STATUS = "outlet2_status"
CONF_OUTLET3_STATUS = "outlet3_status"
CONF_OUTLET4_STATUS = "outlet4_status"
CONF_OUTLET5_STATUS = "outlet5_status"
CONF_OUTLET6_STATUS = "outlet6_status"
CONF_OUTLET7_STATUS = "outlet7_status"
CONF_OUTLET8_STATUS = "outlet8_status"
CONF_OUTLET9_STATUS = "outlet9_status"
CONF_OUTLET10_STATUS = "outlet10_status"

CONF_FAN_NATURAL = "fan_natural"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_SPIDERFARMER_ID): cv.use_id(SpiderfarmerBle),

            cv.Optional(CONF_OUTLET1_STATUS): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_POWER,
            ),
            cv.Optional(CONF_OUTLET2_STATUS): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_POWER,
            ),
            cv.Optional(CONF_OUTLET3_STATUS): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_POWER,
            ),
            cv.Optional(CONF_OUTLET4_STATUS): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_POWER,
            ),
            cv.Optional(CONF_OUTLET5_STATUS): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_POWER,
            ),
            cv.Optional(CONF_OUTLET6_STATUS): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_POWER,
            ),
            cv.Optional(CONF_OUTLET7_STATUS): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_POWER,
            ),
            cv.Optional(CONF_OUTLET8_STATUS): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_POWER,
            ),
            cv.Optional(CONF_OUTLET9_STATUS): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_POWER,
            ),
            cv.Optional(CONF_OUTLET10_STATUS): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_POWER,
            ),

            cv.Optional(CONF_FAN_NATURAL): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_POWER,
            ),
        }
    )
)


async def to_code(config):
    var = await cg.get_variable(config[CONF_SPIDERFARMER_ID])

    if CONF_OUTLET1_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_OUTLET1_STATUS])
        cg.add(var.set_outlet_1_sensor(sens))
    if CONF_OUTLET2_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_OUTLET2_STATUS])
        cg.add(var.set_outlet_2_sensor(sens))
    if CONF_OUTLET3_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_OUTLET3_STATUS])
        cg.add(var.set_outlet_3_sensor(sens))
    if CONF_OUTLET4_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_OUTLET4_STATUS])
        cg.add(var.set_outlet_4_sensor(sens))
    if CONF_OUTLET5_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_OUTLET5_STATUS])
        cg.add(var.set_outlet_5_sensor(sens))
    if CONF_OUTLET6_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_OUTLET6_STATUS])
        cg.add(var.set_outlet_6_sensor(sens))
    if CONF_OUTLET7_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_OUTLET7_STATUS])
        cg.add(var.set_outlet_7_sensor(sens))
    if CONF_OUTLET8_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_OUTLET8_STATUS])
        cg.add(var.set_outlet_8_sensor(sens))
    if CONF_OUTLET9_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_OUTLET9_STATUS])
        cg.add(var.set_outlet_9_sensor(sens))
    if CONF_OUTLET10_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_OUTLET10_STATUS])
        cg.add(var.set_outlet_10_sensor(sens))

    if CONF_FAN_NATURAL in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_FAN_NATURAL])
        cg.add(var.set_fan_natural_sensor(sens))
