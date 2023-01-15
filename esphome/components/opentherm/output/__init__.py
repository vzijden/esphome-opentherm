import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import output
from esphome.const import CONF_ID, CONF_TYPE
from .. import opentherm_ns, OpenThermGateway

CONF_TYPE_CENTRAL_HEATING_ON = "central_heating_on"
CONF_TYPE_BOILER_TARGET_TEMPERATURE = "boiler_target_temperature"

CONF_OPENTHERM_GATEWAY_ID = "opentherm_id"
CONF_OPENTHERM_SENSOR_TYPE = "type"
CONF_BOILER_MAX_TARGET_TEMP = "boiler_max_target_temp"
CONF_BOILER_MIN_TARGET_TEMP = "boiler_min_target_temp"

OpenThermFloatOutput = opentherm_ns.class_(
    "OpenThermFloatOutput", output.FloatOutput, cg.Component
)
OpenThermBinaryOutput = opentherm_ns.class_(
    "OpenThermBinaryOutput", output.BinaryOutput, cg.Component
)

CONFIG_SCHEMA = cv.typed_schema(
    {
        CONF_TYPE_BOILER_TARGET_TEMPERATURE: output.FLOAT_OUTPUT_SCHEMA.extend(
            {
                cv.GenerateID(CONF_OPENTHERM_GATEWAY_ID): cv.use_id(OpenThermGateway),
                cv.Required(CONF_ID): cv.declare_id(OpenThermFloatOutput),
                cv.Optional(CONF_BOILER_MAX_TARGET_TEMP): cv.temperature,
                cv.Optional(CONF_BOILER_MIN_TARGET_TEMP): cv.temperature,
            }
        ),
        CONF_TYPE_CENTRAL_HEATING_ON: output.BINARY_OUTPUT_SCHEMA.extend(
            {
                cv.GenerateID(CONF_OPENTHERM_GATEWAY_ID): cv.use_id(OpenThermGateway),
                cv.GenerateID(): cv.declare_id(OpenThermBinaryOutput),
            }
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_OPENTHERM_GATEWAY_ID])

    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_open_therm_gateway(parent))
    if config[CONF_TYPE] == CONF_TYPE_BOILER_TARGET_TEMPERATURE:
        cg.add(var.set_max_temperature(config[CONF_BOILER_MAX_TARGET_TEMP]))
        cg.add(var.set_min_temperature(config[CONF_BOILER_MIN_TARGET_TEMP]))

    await cg.register_component(var, config)
