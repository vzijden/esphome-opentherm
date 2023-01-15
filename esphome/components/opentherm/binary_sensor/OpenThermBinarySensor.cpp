#include "OpenThermBinarySensor.h"
#include "esphome/components/opentherm/OpenTherm.h"

namespace esphome::opentherm {


void OpenThermBinarySensor::dump_config() { ESP_LOGCONFIG("opentherm.sensor", "Sensor type: %u", sensor_type_); }
void OpenThermBinarySensor::on_response(uint32_t request, uint32_t response) {
  if (OpenTherm::getDataID(request) == Status) {
    if (sensor_type_ == FLAME_ON) {
      publish_state(OpenTherm::isFlameOn(response));
    } else if (sensor_type_ == HEATING_ON) {
      publish_state(OpenTherm::isHotWaterActive(response));
    }
  }
}

}  // namespace esphome::opentherm