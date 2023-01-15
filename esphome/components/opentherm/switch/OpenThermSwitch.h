
#include "esphome/components/switch/switch.h"
#include "../OpenThermGateway.h"

namespace esphome::opentherm {
enum SwitchType {
  GATEWAY_MODE,
  HEATING_OVERRIDE_ON,
  DHW_ON
};

class OpenThermSwitch : public switch_::Switch, public PollingComponent, public OpenThermListener {
 private:
  OpenThermGateway *open_therm_gateway_;
  SwitchType switch_type_;

 public:
  void set_switch_type(SwitchType switch_type) { OpenThermSwitch::switch_type_ = switch_type; }
  void set_open_therm_gateway(OpenThermGateway *open_therm_gateway) { open_therm_gateway_ = open_therm_gateway; }
  void setup() override;

 private:
  void on_response(uint32_t request, uint32_t response) override;

 public:
  void update() override;

 protected:
  void write_state(bool state) override;

};
}


