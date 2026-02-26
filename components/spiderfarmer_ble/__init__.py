import esphome.codegen as cg
from esphome.components import ble_client
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@smurfy"]
MULTI_CONF = True

CONF_SPIDERFARMER_ID = "spiderfarmer_id"

spiderfarmer_ble_ns = cg.esphome_ns.namespace("spiderfarmer_ble")
SpiderfarmerBle = spiderfarmer_ble_ns.class_("SpiderfarmerBle", ble_client.BLEClientNode, cg.PollingComponent)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SpiderfarmerBle),
        }
    )
    .extend(ble_client.BLE_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("15s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)
