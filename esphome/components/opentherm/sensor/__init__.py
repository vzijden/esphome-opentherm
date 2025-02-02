import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import sensor
from .. import opentherm_ns, OpenThermGateway
from esphome.const import (
    STATE_CLASS_MEASUREMENT,
    ICON_THERMOMETER,
    CONF_TYPE,
    UNIT_CELSIUS,
)

CONF_OPENTHERM_SENSOR_ID = "opentherm_id"
CONF_OPENTHERM_SENSOR_TYPE = "type"

OpenThermSensor = opentherm_ns.class_("OpenThermSensor", sensor.Sensor, cg.Component)
OpenThermSensorType = opentherm_ns.enum("OpenThermSensorType")

SENSOR_TYPES = {
    "boiler_temperature": [OpenThermSensorType.BOILER_TEMPERATURE],
    "boiler_return_temperature": [OpenThermSensorType.BOILER_RETURN_TEMPERATURE],
    "relative_modulation_level": [OpenThermSensorType.BOILER_RELATIVE_MODULATION_LEVEL],
    "target_water_temperature": [OpenThermSensorType.BOILER_TARGET_TEMPERATURE],
    "maximum_relative_modulation_level": [
        OpenThermSensorType.MAX_RELATIVE_MODULATION_LEVEL
    ],
}

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        OpenThermSensor,
        icon=ICON_THERMOMETER,
        accuracy_decimals=1,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(
        {
            cv.GenerateID(CONF_OPENTHERM_SENSOR_ID): cv.use_id(OpenThermGateway),
            cv.Required(CONF_OPENTHERM_SENSOR_TYPE): cv.enum(SENSOR_TYPES),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_OPENTHERM_SENSOR_ID])
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)

    cg.add(var.set_sensor_type(config[CONF_OPENTHERM_SENSOR_TYPE]))
    cg.add(var.set_open_therm_gateway(parent))
