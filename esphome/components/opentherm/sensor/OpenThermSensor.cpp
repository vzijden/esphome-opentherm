#include "OpenThermSensor.h"
#include "esphome/components/opentherm/OpenTherm.h"

namespace esphome::opentherm {

void OpenThermSensor::setup() { ESP_LOGCONFIG("opentherm.sensor", "init"); }
void OpenThermSensor::dump_config() { ESP_LOGCONFIG("opentherm.sensor", "Sensor type:"); }
void OpenThermSensor::update() {
  switch (sensor_type_) {
    case BOILER_TEMPERATURE:
      publish_state(read_from_boiler(Tboiler));
      break;
    case BOILER_RETURN_TEMPERATURE:
      publish_state(read_from_boiler(Tret));
      break;
  }
}
OpenThermSensor::OpenThermSensor() : PollingComponent(30000) {}
float OpenThermSensor::read_from_boiler(OpenThermMessageID message_id) {
  return OpenTherm::getFloat(open_therm_master_->send_request_to_boiler_(OpenTherm::buildRequest(READ, message_id, 0)));
}

}  // namespace esphome::opentherm