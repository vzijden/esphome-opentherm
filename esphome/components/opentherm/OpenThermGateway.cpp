//
// Created by Vincent van der Zijden on 18/01/2023.
//

#include "OpenThermGateway.h"
#include "esphome/components/opentherm/OpenTherm.h"
#include "esphome/core/log.h"
using namespace std::placeholders;

namespace esphome::opentherm {
OpenTherm to_boiler = OpenTherm(D6, D5);
OpenTherm to_thermostat = OpenTherm(D2, D1, true);

IRAM_ATTR void handle_boiler_interrupt() { to_boiler.handleInterrupt(); }

IRAM_ATTR void handle_thermostat_interrupt() { to_thermostat.handleInterrupt(); }

static const char *const TAG = "opentherm.gateway";

void OpenThermGateway::setup() {
  to_boiler.begin(handle_boiler_interrupt);

  std::function<void(long unsigned int, OpenThermResponseStatus)> callback =
      std::bind(&OpenThermGateway::handle_thermostat_request, this, _1, _2);

  to_thermostat.begin(handle_thermostat_interrupt, [this](uint32_t request, OpenThermResponseStatus response_status) {
    handle_thermostat_request(request, response_status);
  });
}

void OpenThermGateway::handle_thermostat_request(uint32_t request, OpenThermResponseStatus response_status) {
  auto message_id = OpenTherm::getDataID(request);
  auto message_type = OpenTherm::getMessageType(request);

  //  ESP_LOGD(TAG, "Received request from thermostat: %s %s", OpenTherm::messageTypeToString(message_type),
  //           OpenTherm::messageIdToString(message_id));

  uint32_t response;
  if (message_id == TrOverride && message_type == READ_DATA) {
    ESP_LOGD(TAG, "Received READ TrOverride");
    if (temperature_override_already_set) {
      time_t now;
      time(&now);
      if (wait_override_reset == 0) {
        wait_override_reset = now;
      }

      double time_since_rest = difftime(now, wait_override_reset);
      if (time_since_rest < 90) {
        ESP_LOGI(TAG, "It has been %f seconds since the override reset, returning 0", time_since_rest);
      } else {
        ESP_LOGI(TAG, "It has been %f seconds since the override reset. Thermostat should have been reset.",
                 time_since_rest);
        wait_override_reset = 0;
        temperature_override_already_set = false;
      }
      response = OpenTherm::buildResponse(READ_ACK, TrOverride, OpenTherm::temperatureToData(0));
    } else {
      ESP_LOGI(TAG, "Returning override setpoint %f", temperature_override_);
      response = OpenTherm::buildResponse(READ_ACK, TrOverride, OpenTherm::temperatureToData(temperature_override_));
    }

//  } else if (message_id == RemoteOverrideFunction && message_type == READ_DATA) {
//    unsigned int data = true;
//    data <<= 8;
//    response = OpenTherm::buildResponse(READ_ACK, RemoteOverrideFunction, data);
  } else {
    response = to_boiler.sendRequest(request);
  }

   ESP_LOGD(TAG, "Responding with %s %s %f", OpenTherm::messageTypeToString(OpenTherm::getMessageType(response)),
            OpenTherm::messageIdToString(message_id), OpenTherm::getFloat(response));

  for (const auto &listener : listeners) {
    listener->on_response(request, response);
  }

  to_thermostat.sendResponse(response);
}
void OpenThermGateway::update() {}

void OpenThermGateway::set_temperature_setpoint_override(float temperature) {
  temperature_override_already_set = true;
  temperature_override_ = temperature;

}
void OpenThermGateway::loop() { to_thermostat.process(); }
void OpenThermGateway::add_listener(OpenThermListener *listener) { listeners.push_back(listener); }

uint32_t OpenThermGateway::send_request_to_thermostat(uint32_t request) { return to_thermostat.sendRequest(request); }

uint32_t OpenThermGateway::send_request_to_boiler(uint32_t request) {
  uint32_t response = to_boiler.sendRequest(request);
  if (OpenTherm::getMessageType(response) == DATA_INVALID || OpenTherm::getMessageType(response) == UNKNOWN_DATA_ID) {
    ESP_LOGW(TAG, "Got response %s from boiler to request %s",
             OpenTherm::messageTypeToString(OpenTherm::getMessageType(response)),
             OpenTherm::messageIdToString(OpenTherm::getDataID(response)));
  }
  return response;
}
}  // namespace esphome::opentherm