#pragma once

#include "esphome/components/climate/climate.h"
#include "../OpenThermGateway.h"

namespace esphome::opentherm {

class OpenThermClimate : public climate::Climate, public Component {
 private:
  OpenThermGateway *open_therm_gateway_;

 public:
  void set_open_therm_gateway(OpenThermGateway *open_therm_gateway) { open_therm_gateway_ = open_therm_gateway; }

 protected:
  climate::ClimateTraits traits() override {
    auto traits = climate::ClimateTraits();

    traits.set_supports_action(true);
    traits.set_supported_modes({
        climate::CLIMATE_MODE_HEAT,
    });
    traits.set_supports_current_temperature(true);

    return traits;
  }
  void control(const climate::ClimateCall &call) override;
};

}  // namespace esphome::opentherm