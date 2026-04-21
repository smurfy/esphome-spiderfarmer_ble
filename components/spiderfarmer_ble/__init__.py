import re
import esphome.codegen as cg
from esphome.components import ble_client
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@smurfy"]
MULTI_CONF = True

CONF_SPIDERFARMER_ID = "spiderfarmer_id"
CONF_SPIDERFARMER_AES_KEY = "spiderfarmer_aes_key"
CONF_SPIDERFARMER_AES_IV = "spiderfarmer_aes_iv"

def validate_aes_data(value):
    value = cv.string(value)
    match = re.match(r"^[a-z0-9A-Z]{16}$", value)
    if match is None:
        raise cv.Invalid(f"{value} is not a valid AES value, make sure its 16 byte string")
    return value

spiderfarmer_ble_ns = cg.esphome_ns.namespace("spiderfarmer_ble")
SpiderfarmerBle = spiderfarmer_ble_ns.class_("SpiderfarmerBle", ble_client.BLEClientNode, cg.PollingComponent)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SpiderfarmerBle),

            cv.Optional(CONF_SPIDERFARMER_AES_KEY): validate_aes_data,
            cv.Optional(CONF_SPIDERFARMER_AES_IV): validate_aes_data,
        }
    )
    .extend(ble_client.BLE_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("15s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    if CONF_SPIDERFARMER_AES_KEY in config:
        cg.add(var.set_aes_key(config[CONF_SPIDERFARMER_AES_KEY]))
    if CONF_SPIDERFARMER_AES_IV in config:
        cg.add(var.set_aes_iv(config[CONF_SPIDERFARMER_AES_IV]))
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)
