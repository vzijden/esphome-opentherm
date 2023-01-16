//
// Created by Vincent van der Zijden on 11/01/2023.
//

#include "OpenThermBinaryOutput.h"

namespace esphome {
namespace opentherm {

void OpenThermBinaryOutput::write_state(bool state) {
  switch (open_therm_binary_output_type_) {
    case HEATING:
      open_therm_master_->set_heating(state);
      break;
    case HOT_WATER:
      open_therm_master_->set_dhw(state);
      break;
  }
}
void OpenThermBinaryOutput::on_response(uint32_t request, uint32_t response) {
  if (match_message_type(response)) {
    switch (open_therm_binary_output_type_) {
      case HEATING:
        set_state(OpenTherm::isCentralHeatingActive(response));
        break;
      case HOT_WATER:
        set_state(OpenTherm::isHotWaterActive(response));
        break;
    }
  }
}
bool OpenThermBinaryOutput::match_message_type(uint32_t messageId) { return messageId == Status; }

}  // namespace opentherm
}  // namespace esphome