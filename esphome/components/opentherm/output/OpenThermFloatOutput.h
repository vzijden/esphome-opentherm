#pragma once

#include "esphome/components/output/float_output.h"
#include "../OpenThermGateway.h"

namespace esphome::opentherm {

class OpenThermFloatOutput : public output::FloatOutput, public Component {
 private:
  OpenThermGateway *open_therm_gateway_;
  char max_temperature_;
  char min_temperature_;

 public:
  void write_state(float state) override;
  void set_open_therm_gateway(OpenThermGateway *open_therm_gateway) { open_therm_gateway_ = open_therm_gateway;
  }
  float map_to_temperature_range(float state) const;
  void set_max_temperature(char max_temperature);
  void set_min_temperature(char min_temperature);
};

}  // namespace esphome::opentherm
