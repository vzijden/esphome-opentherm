import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import output
from esphome.const import CONF_ID, CONF_TYPE
from .. import opentherm_ns, OpenThermMaster

CONF_TYPE_HEATING = "heating"
CONF_TYPE_HOT_WATER = "hot_water"
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
OpenThermBinaryOutputType = opentherm_ns.enum("OpenThermBinaryOutputType")

CONFIG_SCHEMA = cv.typed_schema(
    {
        CONF_TYPE_BOILER_TARGET_TEMPERATURE: output.FLOAT_OUTPUT_SCHEMA.extend(
            {
                cv.GenerateID(CONF_OPENTHERM_GATEWAY_ID): cv.use_id(OpenThermMaster),
                cv.Required(CONF_ID): cv.declare_id(OpenThermFloatOutput),
            }
        ),
        CONF_TYPE_HEATING: output.BINARY_OUTPUT_SCHEMA.extend(
            {
                cv.GenerateID(CONF_OPENTHERM_GATEWAY_ID): cv.use_id(OpenThermMaster),
                cv.GenerateID(): cv.declare_id(OpenThermBinaryOutput),
            }
        ),
        CONF_TYPE_HOT_WATER: output.BINARY_OUTPUT_SCHEMA.extend(
            {
                cv.GenerateID(CONF_OPENTHERM_GATEWAY_ID): cv.use_id(OpenThermMaster),
                cv.GenerateID(): cv.declare_id(OpenThermBinaryOutput),
            }
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_OPENTHERM_GATEWAY_ID])

    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_open_therm_master(parent))

    if config[CONF_TYPE] == CONF_TYPE_HEATING:
        cg.add(var.set_type(OpenThermBinaryOutputType.HEATING))
    if config[CONF_TYPE] == CONF_TYPE_HOT_WATER:
        cg.add(var.set_type(OpenThermBinaryOutputType.HOT_WATER))

    await cg.register_component(var, config)
