#include "OpenThermNumber.h"
#include "esphome/components/opentherm/OpenTherm.h"
#include "esphome/core/log.h"

namespace esphome::opentherm {

void OpenThermNumber::setup() {
  ESP_LOGCONFIG("opentherm.number", "init");
}
void OpenThermNumber::dump_config() { ESP_LOGCONFIG("opentherm.number", "Number type:"); }

void OpenThermNumber::control(float value) {
  if (this->number_type_ == WATER_TARGET_TEMP) {
    open_therm_gateway_->set_target_water_temp(value);
  }
  this->publish_state(value);
}

}  // namespace esphome::opentherm