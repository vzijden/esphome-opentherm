#include "OpenThermSensor.h"
#include "esphome/components/opentherm/OpenTherm.h"

namespace esphome::opentherm {

bool match_open_therm_message(uint32_t request, uint32_t response, OpenThermSensorType sensor_type) {
  switch (sensor_type) {
    case THERMOSTAT_ROOM_TEMPERATURE:
      return OpenTherm::getDataID(request) == Tr;
    case BOILER_TARGET_TEMPERATURE:
      return OpenTherm::getDataID(request) == TSet;
  }

  return false;
}

void OpenThermSensor::setup() { ESP_LOGCONFIG("opentherm.sensor", "init"); }
void OpenThermSensor::dump_config() { ESP_LOGCONFIG("opentherm.sensor", "Sensor type:"); }
void OpenThermSensor::on_response(uint32_t request, uint32_t response) {
  if (match_open_therm_message(request, response, sensor_type_)) {
    publish_state(OpenTherm::getFloat(response));
  }
}
void OpenThermSensor::update() {
  switch (sensor_type_) {
    case BOILER_TEMPERATURE:
      publish_state(OpenTherm::getFloat(open_therm_gateway_->read_from_boiler(Tboiler)));
      break;
    case BOILER_RETURN_TEMPERATURE:
      publish_state(OpenTherm::getFloat(open_therm_gateway_->read_from_boiler(Tret)));
      break;
  }
}
OpenThermSensor::OpenThermSensor() : PollingComponent(30000) {}

}  // namespace esphome::opentherm