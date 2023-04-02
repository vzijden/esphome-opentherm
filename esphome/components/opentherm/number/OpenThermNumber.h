#pragma once

#include "esphome/components/number/number.h"
#include "esphome/components/opentherm/OpenThermListener.h"
#include "../OpenTherm.h"
#include "../OpenThermGateway.h"
namespace esphome::opentherm {

enum OpenThermNumberType { WATER_TARGET_TEMP };

class OpenThermNumber : public number::Number, public Component {
 public:
  void setup() override;
  void dump_config() override;

  void set_number_type(OpenThermNumberType number_type) { OpenThermNumber::number_type_ = number_type; }

 private:
  OpenThermNumberType number_type_;
  OpenThermGateway *open_therm_gateway_;

 public:
  void set_open_therm_gateway(OpenThermGateway *open_therm_gateway) {
    OpenThermNumber::open_therm_gateway_ = open_therm_gateway;
  }

 protected:
  void control(float value) override;
};

}  // namespace esphome::opentherm
