from homeassistant.const import CONF_NAME

import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import number
from .. import opentherm_ns, OpenThermGateway

CONF_OPENTHERM_INPUT_NUMBER_ID = "opentherm_id"
CONF_OPENTHERM_input_number_TYPE = "type"
ATTR_MIN = "min_value"
ATTR_MAX = "max_value"

OpenThermNumber = opentherm_ns.class_("OpenThermNumber", number.Number, cg.Component)
OpenThermNumberType = opentherm_ns.enum("OpenThermNumberType")

input_number_TYPES = {
    "water_target_temp": [OpenThermNumberType.WATER_TARGET_TEMP],
}

CONFIG_SCHEMA = number.NUMBER_SCHEMA.extend(
    {
        cv.Optional(CONF_NAME): cv.string,
        cv.GenerateID(CONF_OPENTHERM_INPUT_NUMBER_ID): cv.use_id(OpenThermGateway),
        cv.Required(CONF_OPENTHERM_input_number_TYPE): cv.enum(input_number_TYPES),
        cv.GenerateID(): cv.declare_id(OpenThermNumber),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_OPENTHERM_INPUT_NUMBER_ID])
    var = await number.new_number(config, min_value=60, max_value=90, step=1)
    await cg.register_component(var, config)

    cg.add(var.set_number_type(config[CONF_OPENTHERM_input_number_TYPE]))
    cg.add(var.set_open_therm_gateway(parent))
