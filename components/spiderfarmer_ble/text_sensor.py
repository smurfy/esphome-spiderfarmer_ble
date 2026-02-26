import esphome.codegen as cg
from esphome.components import ble_client, text_sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_DEVICE_ID,
    ENTITY_CATEGORY_DIAGNOSTIC,
    ICON_CHIP
)

from . import CONF_SPIDERFARMER_ID, SpiderfarmerBle

DEPENDENCIES = ["spiderfarmer_ble"]

CONF_FWVERSION = "fw_version"
CONF_HWVERSION = "hw_version"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SPIDERFARMER_ID): cv.use_id(SpiderfarmerBle),
        cv.Required(CONF_DEVICE_ID): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon=ICON_CHIP
        ),
        cv.Required(CONF_FWVERSION): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon=ICON_CHIP
        ),
        cv.Required(CONF_HWVERSION): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon=ICON_CHIP
        ),
    }
)

async def to_code(config):
    var = await cg.get_variable(config[CONF_SPIDERFARMER_ID])

    if CONF_DEVICE_ID in config:
        sens = await text_sensor.new_text_sensor(config[CONF_DEVICE_ID])
        cg.add(var.set_deviceid_sensor(sens))
    if CONF_FWVERSION in config:
        sens = await text_sensor.new_text_sensor(config[CONF_FWVERSION])
        cg.add(var.set_fwversion_sensor(sens))
    if CONF_HWVERSION in config:
        sens = await text_sensor.new_text_sensor(config[CONF_HWVERSION])
        cg.add(var.set_hwversion_sensor(sens))