#include "OpenThermBinarySensor.h"
#include "esphome/components/opentherm/OpenTherm.h"
#include "esphome/core/log.h"

namespace esphome::opentherm {

void OpenThermBinarySensor::dump_config() { ESP_LOGCONFIG("opentherm.sensor", "Sensor type: %u", sensor_type_); }
void OpenThermBinarySensor::setup() { open_therm_gateway_->add_listener(this); }

void OpenThermBinarySensor::on_response(uint32_t request, uint32_t response) {
  if (sensor_type_ == FLAME_ON) {
    publish_state(OpenTherm::isFlameOn(response));
  } else if (sensor_type_ == THERMOSTAT_HEATING_ON) {
    publish_state(OpenTherm::isCentralHeatingActive(request));
  }
}

}  // namespace esphome::opentherm