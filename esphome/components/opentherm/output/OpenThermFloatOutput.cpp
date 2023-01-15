//
// Created by Vincent van der Zijden on 11/01/2023.
//

#include "OpenThermFloatOutput.h"

namespace esphome::opentherm {

float OpenThermFloatOutput::map_to_temperature_range(float state) const {
  return min_temperature_ + (max_temperature_ - min_temperature_) * state;
}

void OpenThermFloatOutput::write_state(float state) {
  open_therm_gateway_->send_write_to_boiler(TSet, OpenTherm::temperatureToData(state));
}
void OpenThermFloatOutput::set_max_temperature(char max_temperature) { max_temperature_ = max_temperature; }
void OpenThermFloatOutput::set_min_temperature(char min_temperature) { min_temperature_ = min_temperature; }

}  // namespace esphome::opentherm