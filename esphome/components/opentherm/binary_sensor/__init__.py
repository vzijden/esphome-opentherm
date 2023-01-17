import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from .. import opentherm_ns, OpenThermMaster

CONF_OPENTHERM_BINARY_SENSOR_ID = "opentherm_id"
CONF_OPENTHERM_BINARY_SENSOR_TYPE = "type"

OpenThermBinarySensor = opentherm_ns.class_(
    "OpenThermBinarySensor", binary_sensor.BinarySensor, cg.Component
)
OpenThermBinarySensorType = opentherm_ns.enum("OpenThermBinarySensorType")

SENSOR_TYPES = {
    "flame_on": [OpenThermBinarySensorType.FLAME_ON],
}

CONFIG_SCHEMA = binary_sensor.binary_sensor_schema(OpenThermBinarySensor).extend(
    {
        cv.GenerateID(CONF_OPENTHERM_BINARY_SENSOR_ID): cv.use_id(OpenThermMaster),
        cv.Required(CONF_OPENTHERM_BINARY_SENSOR_TYPE): cv.enum(SENSOR_TYPES),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_OPENTHERM_BINARY_SENSOR_ID])
    var = await binary_sensor.new_binary_sensor(config)
    await cg.register_component(var, config)

    cg.add(var.set_sensor_type(config[CONF_OPENTHERM_BINARY_SENSOR_TYPE]))
    cg.add(var.set_open_therm_master(parent))
