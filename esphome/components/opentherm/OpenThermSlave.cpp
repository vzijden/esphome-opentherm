
#include "esphome/core/log.h"
#include "OpenThermSlave.h"

OpenTherm THERMOSTAT_SLAVE = OpenTherm(D2, D1, true);

namespace esphome::opentherm {

IRAM_ATTR void handle_interrupt_() { THERMOSTAT_SLAVE.handleInterrupt(); }

void OpenThermSlave::process_request_(unsigned long request, OpenThermResponseStatus status) {
  auto data_id = OpenTherm::getDataID(request);
  if (data_id == Tr && on_room_temperature_updated_) {
    on_room_temperature_updated_(OpenTherm::getFloat(request));
  }

  if (on_thermostat_request_callback_ != nullptr) {
    const uint32_t response = on_thermostat_request_callback_(request);
    THERMOSTAT_SLAVE.sendResponse(response);
  }
}

OpenThermSlave::OpenThermSlave(int slave_in_pin, int slave_out_pin) {
  ESP_LOGI("opentherm.slave", "init");

  THERMOSTAT_SLAVE.begin(handle_interrupt_, [this](unsigned long request, OpenThermResponseStatus status) {
    process_request_(request, status);
  });
}

void OpenThermSlave::loop() { THERMOSTAT_SLAVE.process(); }

}  // namespace esphome::opentherm