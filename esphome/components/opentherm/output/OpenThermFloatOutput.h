#pragma once

#include "esphome/components/output/float_output.h"
#include "esphome/components/opentherm/OpenThermMaster.h"

namespace esphome::opentherm {

class OpenThermFloatOutput : public output::FloatOutput, public Component {
 private:
  OpenThermMaster *open_therm_master_;
  char max_temperature_;
  char min_temperature_;

 public:
  void write_state(float state) override;
  void set_open_therm_master(OpenThermMaster *open_therm_master) { this->open_therm_master_ = open_therm_master; }
  float map_to_temperature_range(float state) const;
  void setup() override;
};

}  // namespace esphome::opentherm
