//
// Created by Vincent van der Zijden on 15/01/2023.
//

#include "OpenThermSwitch.h"

namespace esphome::opentherm {

void OpenThermSwitch::write_state(bool state) {
  switch (switch_type_) {
    case GATEWAY_MODE:
      open_therm_gateway_->set_gateway_mode(state);
      break;
    case DHW_ON:
      open_therm_gateway_->set_hot_water(state);
      break;
  }

  this->publish_state(state);
}

void OpenThermSwitch::setup() { publish_state(true); }
void OpenThermSwitch::on_response(uint32_t request, uint32_t response) {
  if (OpenTherm::getDataID(request) == Status) {
     if (switch_type_ == DHW_ON) {
      publish_state(OpenTherm::isHotWaterActive(response));
    }
  }
}
void OpenThermSwitch::update() {
  open_therm_gateway_->read_from_boiler()
}
}  // namespace esphome::opentherm