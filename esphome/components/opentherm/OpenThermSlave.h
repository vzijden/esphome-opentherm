#pragma once

#include "esphome/core/component.h"
#include "esphome/components/opentherm/OpenTherm.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/helpers.h"

namespace esphome::opentherm {

class OpenThermSlave{
 private:
  std::function<uint32_t (uint32_t)> on_thermostat_request_callback_;
  void (*on_room_temperature_updated_)(float);

 public:
  OpenThermSlave(int slave_in_pin, int slave_out_pin);
  void set_on_thermostat_request_callback(std::function<uint32_t (uint32_t )> &&on_thermostat_request_callback) {
    on_thermostat_request_callback_ = on_thermostat_request_callback;
  }
  void set_on_room_temperature_updated(void (*on_room_temperature_updated)(float)) {
    OpenThermSlave::on_room_temperature_updated_ = on_room_temperature_updated;
  }
  void loop();
  void process_request_(unsigned long request, OpenThermResponseStatus status);
};

}  // namespace esphome::opentherm
