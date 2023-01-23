import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import switch
from .. import opentherm_ns, OpenThermGateway

CONF_OPENTHERM_SWITCH_ID = "opentherm_id"
CONF_OPENTHERM_SWITCH_TYPE = "type"

OpenThermSwitch = opentherm_ns.class_("OpenThermSwitch", switch.Switch, cg.Component)
OpenThermSwitchType = opentherm_ns.enum("OpenThermSwitchType")

SWITCH_TYPES = {
    "boiler_override": [OpenThermSwitchType.BOILER_OVERRIDE],
}

CONFIG_SCHEMA = (
    switch.switch_schema(OpenThermSwitch)
    .extend(
        {
            cv.GenerateID(CONF_OPENTHERM_SWITCH_ID): cv.use_id(OpenThermGateway),
            cv.Required(CONF_OPENTHERM_SWITCH_TYPE): cv.enum(SWITCH_TYPES),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_OPENTHERM_SWITCH_ID])
    var = await switch.new_switch(config)
    await cg.register_component(var, config)

    cg.add(var.set_switch_type(config[CONF_OPENTHERM_SWITCH_TYPE]))
    cg.add(var.set_open_therm_gateway(parent))
