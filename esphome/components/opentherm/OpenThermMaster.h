#pragma once

#include "esphome/core/component.h"
#include "OpenTherm.h"

namespace esphome::opentherm {

class OpenThermMaster : public Component {
 private:
  int master_in_pin_{0};
  int master_out_pin_{0};

 public:
  void set_master_in_pin(int master_in_pin);
  void set_master_out_pin(int master_out_pin);

 private:
  bool heating_on_;
  bool DHW_on_;

 public:
  void setup() override;

  void set_target_temperature(float temperature) const;
  uint32_t handle_thermostat_request(uint32_t request);
  uint32_t send_request_to_boiler_(uint32_t request) const;

  void set_heating(bool heating_on);
  void set_dhw(bool dhw_on);

 private:
  void update_status();
};

}  // namespace esphome::opentherm