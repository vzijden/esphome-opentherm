import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

opentherm_ns = cg.esphome_ns.namespace("opentherm")
OpenThermGateway = opentherm_ns.class_("OpenThermGateway", cg.PollingComponent)

CONF_MASTER_IN_PIN = "master_in_pin"
CONF_MASTER_OUT_PIN = "master_out_pin"
CONF_SLAVE_IN_PIN = "slave_in_pin"
CONF_SLAVE_OUT_PIN = "slave_out_pin"

CONFIG_SCHEMA = cv.Schema(
    cv.COMPONENT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(OpenThermGateway),
            cv.Optional(CONF_MASTER_IN_PIN): cv.positive_int,
            cv.Optional(CONF_MASTER_OUT_PIN): cv.positive_int,
            cv.Optional(CONF_SLAVE_IN_PIN): cv.positive_int,
            cv.Optional(CONF_SLAVE_OUT_PIN): cv.positive_int,
        }
    )
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if CONF_MASTER_IN_PIN in config:
        cg.add(var.set_master_in_pin(config[CONF_MASTER_IN_PIN]))
        cg.add(var.set_master_out_pin(config[CONF_MASTER_OUT_PIN]))
    if CONF_SLAVE_IN_PIN in config:
        cg.add(var.set_slave_in_pin(config[CONF_SLAVE_IN_PIN]))
        cg.add(var.set_slave_out_pin(config[CONF_SLAVE_OUT_PIN]))
