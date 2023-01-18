//
// Created by Vincent van der Zijden on 18/01/2023.
//

#include "OpenThermClimate.h"
void esphome::opentherm::OpenThermClimate::control(const esphome::climate::ClimateCall &call) {
  if (call.get_target_temperature().has_value()) {
    open_therm_gateway_->set_temperature_setpoint_override(call.get_target_temperature().value());
    this->target_temperature = call.get_target_temperature().value();

  }
  this->publish_state();
}
