//
// Created by Vincent van der Zijden on 11/01/2023.
//

#include "OpenThermBinaryOutput.h"

namespace esphome {
namespace opentherm {
void OpenThermBinaryOutput::write_state(bool state) { open_therm_gateway_->set_heating_on(state); }

}  // namespace opentherm
}  // namespace esphome