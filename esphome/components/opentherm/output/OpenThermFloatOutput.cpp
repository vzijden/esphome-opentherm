//
// Created by Vincent van der Zijden on 11/01/2023.
//

#include "OpenThermFloatOutput.h"
#include "esphome/core/log.h"

namespace esphome::opentherm {

void OpenThermFloatOutput::write_state(float state) {
  unsigned int data = OpenTherm::temperatureToData(state);
  unsigned long temp = OpenTherm::buildRequest(WRITE_DATA, TSet, data);
  ESP_LOGD("opentherm.floatOutput", "Setting boiler temp to %u", data);
  open_therm_master_->send_request_to_boiler_(temp);

}
void OpenThermFloatOutput::setup() {
//  open_therm_master_->send_request_to_boiler_(
//      OpenTherm::buildRequest(WRITE, TSet, OpenTherm::temperatureToData(gets)));
}
}  // namespace esphome::opentherm
   // namespace esphome::opentherm