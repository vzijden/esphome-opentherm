#pragma once

#include "esphome/components/switch/switch.h"
#include "esphome/components/opentherm/OpenThermListener.h"
#include "../OpenTherm.h"
#include "../OpenThermGateway.h"
namespace esphome::opentherm {

enum OpenThermSwitchType { BOILER_OVERRIDE };

class OpenThermSwitch : public switch_ ::Switch, public Component {
 public:
  void setup() override;
  void dump_config() override;

  void set_switch_type(OpenThermSwitchType switch_type) { OpenThermSwitch::switch_type_ = switch_type; }

 protected:
  void write_state(bool state) override;

 public:

 private:
  OpenThermSwitchType switch_type_;
  OpenThermGateway *open_therm_gateway_;

 public:
  void set_open_therm_gateway(OpenThermGateway *open_therm_gateway) {
    OpenThermSwitch::open_therm_gateway_ = open_therm_gateway;
  }
};

}  // namespace esphome::opentherm
