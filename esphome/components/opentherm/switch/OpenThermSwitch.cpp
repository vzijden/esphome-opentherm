#include "OpenThermSwitch.h"
#include "esphome/components/opentherm/OpenTherm.h"
#include "esphome/core/log.h"

namespace esphome::opentherm {

void OpenThermSwitch::setup() {
  ESP_LOGCONFIG("opentherm.switch", "init");
}
void OpenThermSwitch::dump_config() { ESP_LOGCONFIG("opentherm.switch", "Switch type:"); }

void OpenThermSwitch::write_state(bool state) {
  open_therm_gateway_->set_boiler_on_override(state);
  publish_state(state);
}

}  // namespace esphome::opentherm