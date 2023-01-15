
#include "OpenThermMaster.h"
namespace esphome::opentherm {

OpenTherm to_boiler = OpenTherm(D6, D5);

IRAM_ATTR void handle_interrupt() { to_boiler.handleInterrupt(); }

uint32_t OpenThermMaster::handle_thermostat_request(uint32_t request) {
  uint32_t const response = to_boiler.sendRequest(request);

  return response;
}



OpenThermMaster::OpenThermMaster(int master_in_pin, int master_out_pin)
    : master_in_pin_(master_in_pin), master_out_pin_(master_out_pin) {
  ESP_LOGD("opentherm.master", "initializing");

//  auto f = std::bind(&OpenThermMaster::handle_interrupt, this);
  to_boiler.begin(handle_interrupt);
}

void OpenThermMaster::set_target_temperature(float temperature) const {
  auto temp_data = OpenTherm::temperatureToData(temperature);
  auto request = to_boiler.buildRequest(WRITE, TSet, temp_data);
  send_request_to_boiler_(request);
}


void OpenThermMaster::set_max_relative_modulation_level(unsigned int max_modulation_level) {
  auto request = to_boiler.buildRequest(WRITE, MaxRelModLevelSetting, max_modulation_level);
  send_request_to_boiler_(request);
}

unsigned long OpenThermMaster::send_request_to_boiler_(uint32_t request) const {
  auto response = to_boiler.sendRequest(request);
  auto response_type = OpenTherm::getMessageType(response);
  if ( response_type != WRITE_ACK && response_type != READ_ACK) {
    ESP_LOGW("opentherm.master", "Error response from boiler to request %s to request %s",
             to_boiler.messageTypeToString(response_type), OpenTherm::messageIdToString(OpenTherm::getDataID(response)));
  }
  return response;
}

char OpenThermMaster::getStatus() {
  return to_boiler.status;
}

}  // namespace esphome::opentherm