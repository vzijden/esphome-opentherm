import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import climate
from esphome.const import CONF_ID, CONF_TYPE
from .. import opentherm_ns, OpenThermGateway

CONF_OPENTHERM_GATEWAY_ID = "opentherm_id"

OpenThermClimate = opentherm_ns.class_(
    "OpenThermClimate", climate.Climate, cg.Component
)

CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend(
    {
        cv.GenerateID(CONF_OPENTHERM_GATEWAY_ID): cv.use_id(OpenThermGateway),
        cv.Required(CONF_ID): cv.declare_id(OpenThermClimate),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_OPENTHERM_GATEWAY_ID])

    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_open_therm_gateway(parent))

    await climate.register_climate(var, config)
