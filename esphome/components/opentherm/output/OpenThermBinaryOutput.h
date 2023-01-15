#pragma once
#include "esphome/components/output/binary_output.h"
#include "../OpenThermGateway.h"
namespace esphome::opentherm {

class OpenThermBinaryOutput : public output::BinaryOutput, public Component {
 private:
  OpenThermGateway *open_therm_gateway_;

 public:
  void set_open_therm_gateway(OpenThermGateway *open_therm_gateway) { open_therm_gateway_ = open_therm_gateway;
  }

 protected:
  void write_state(bool state) override;
};

}  // namespace esphome::opentherm
