import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import switch
from esphome.const import CONF_ID, CONF_TYPE
from .. import opentherm_ns, OpenThermGateway

CONF_TYPE_GATEWAY_MODE = "gateway_mode"

OpenthermSwitch = opentherm_ns.class_("OpenThermSwitch", switch.Switch, cg.Component)
OpenThermSwitchType = opentherm_ns.enum("SwitchType")

SWITCH_TYPES = {
    "gateway_mode": [OpenThermSwitchType.GATEWAY_MODE],
    "heating_override_on": [OpenThermSwitchType.HEATING_OVERRIDE_ON],
    "dhw_on": [OpenThermSwitchType.DHW_ON],
}


CONF_OPENTHERM_GATEWAY_ID = "opentherm_id"
CONF_OPENTHERM_SWITCH_TYPE = "type"
CONF_BOILER_MAX_TARGET_TEMP = "boiler_max_target_temp"
CONF_BOILER_MIN_TARGET_TEMP = "boiler_min_target_temp"


CONFIG_SCHEMA = switch.switch_schema(default_restore_mode="RESTORE_DEFAULT_ON").extend(
    {
        cv.GenerateID(CONF_OPENTHERM_GATEWAY_ID): cv.use_id(OpenThermGateway),
        cv.Required(CONF_ID): cv.declare_id(OpenthermSwitch),
        cv.Required(CONF_OPENTHERM_SWITCH_TYPE): cv.enum(SWITCH_TYPES),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_OPENTHERM_GATEWAY_ID])

    var = await switch.new_switch(config)
    await cg.register_component(var, config)
    cg.add(var.set_open_therm_gateway(parent))
    cg.add(var.set_switch_type(config[CONF_OPENTHERM_SWITCH_TYPE]))

    cg.add(parent.register_sensor(var))
