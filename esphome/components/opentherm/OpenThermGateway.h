#pragma once

#include "esphome/core/component.h"
#include "OpenThermListener.h"
#include "OpenTherm.h"
namespace esphome::opentherm {
class OpenThermGateway : public PollingComponent {
 private:
  std::vector<OpenThermListener *> listeners{std::vector<OpenThermListener *>()};
  float temperature_override_;
  time_t wait_override_reset = 0;
  bool override_active;

 public:
  void update() override;

 private:
  bool temperature_override_already_set;

  void handle_thermostat_request(uint32_t request, OpenThermResponseStatus response_status);

 public:
  void add_listener(OpenThermListener *listener);
  void set_temperature_setpoint_override(float temperature);
  void loop() override;
  void setup() override;
  uint32_t send_request_to_thermostat(uint32_t request);
  uint32_t send_request_to_boiler(uint32_t request);
};
}  // namespace esphome::opentherm