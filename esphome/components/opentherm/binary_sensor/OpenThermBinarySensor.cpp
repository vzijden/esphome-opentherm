#include "OpenThermBinarySensor.h"
#include "esphome/components/opentherm/OpenTherm.h"
#include "esphome/core/log.h"

namespace esphome::opentherm {

void OpenThermBinarySensor::dump_config() { ESP_LOGCONFIG("opentherm.sensor", "Sensor type: %u", sensor_type_); }
OpenThermBinarySensor::OpenThermBinarySensor() : PollingComponent(30000) {}

void OpenThermBinarySensor::update() {
  auto response = open_therm_master_->update_status();
  if (sensor_type_ == FLAME_ON) {
    publish_state(OpenTherm::isFlameOn(response));
  }
}

}  // namespace esphome::opentherm