
#include "OpenThermMaster.h"
#include "esphome/core/log.h"
namespace esphome::opentherm {

OpenTherm to_boiler = OpenTherm(D6, D5);

IRAM_ATTR void handle_interrupt() { to_boiler.handleInterrupt(); }

uint32_t OpenThermMaster::handle_thermostat_request(uint32_t request) {
  uint32_t const response = to_boiler.sendRequest(request);

  return response;
}

void OpenThermMaster::set_target_temperature(float temperature) const {
  auto temp_data = OpenTherm::temperatureToData(temperature);
  auto request = to_boiler.buildRequest(WRITE, TSet, temp_data);
  send_request_to_boiler_(request);
}

uint32_t OpenThermMaster::send_request_to_boiler_(uint32_t request) const {
  auto response = to_boiler.sendRequest(request);
  auto response_type = OpenTherm::getMessageType(response);
  if (response_type != WRITE_ACK && response_type != READ_ACK) {
    ESP_LOGW("opentherm.master", "Error response from boiler to request %s to request %s",
             to_boiler.messageTypeToString(response_type),
             OpenTherm::messageIdToString(OpenTherm::getDataID(response)));
  }
  return response;
}
void OpenThermMaster::setup() { to_boiler.begin(handle_interrupt); }

void OpenThermMaster::set_dhw(bool dhw_on) {
  DHW_on_ = dhw_on;
  update_status();
}
void OpenThermMaster::set_heating(bool heating_on) {
  heating_on_ = heating_on;
  update_status();
}
void OpenThermMaster::update_status() { to_boiler.setBoilerStatus(heating_on_, DHW_on_); }
void OpenThermMaster::set_master_in_pin(int master_in_pin) { master_in_pin_ = master_in_pin; }
void OpenThermMaster::set_master_out_pin(int master_out_pin) { master_out_pin_ = master_out_pin; }

}  // namespace esphome::opentherm