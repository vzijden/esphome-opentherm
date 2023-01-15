#pragma once

#include "esphome/core/component.h"
//#include "esphome/components/opentherm/OpenTherm.h"
#include "esphome/components/sensor/sensor.h"
#include "OpenThermSlave.h"

namespace esphome::opentherm {

class OpenThermMaster {
 private:
  sensor::Sensor *boiler_temp_sensor_{nullptr};
  sensor::Sensor *boiler_target_temp_sensor_{nullptr};
  int master_in_pin_{0};
  int master_out_pin_{0};

 public:
  OpenThermMaster(int master_in_pin, int master_out_pin);
//  void handle_interrupt() const;

  void set_target_temperature(float temperature) const;
  uint32_t handle_thermostat_request(uint32_t request);

  unsigned long send_request_to_boiler_(uint32_t request) const;
  void set_heating_on(bool heating_on);
  void set_domestic_hot_water_on(bool domestic_hot_water_on);
  void set_max_relative_modulation_level(unsigned int max_modulation_level);
  char getStatus();
};

}  // namespace esphome::opentherm