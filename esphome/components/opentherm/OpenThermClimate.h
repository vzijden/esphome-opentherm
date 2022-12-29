#pragma once

#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/hal.h"
#include "esphome/components/climate/climate.h"

namespace esphome {

class OpenThermClimate : public climate::Climate, public Component {
 public:
  std::function<void(int)> tempSetPointCallback;

  void setup() override {
    // This will be called by App.setup()
  }

  void registerTempSetpointCallback(std::function<void(int)> callback) { tempSetPointCallback = callback; }

  void control(const climate::ClimateCall &call) override {
    if (call.get_mode().has_value()) {
      // User requested mode change
      climate::ClimateMode mode = *call.get_mode();
      // Send mode to hardware
      // ...

      // Publish updated state
      this->mode = mode;
      this->publish_state();
    }
    if (call.get_target_temperature().has_value()) {
      float temp = *call.get_target_temperature();

      if (tempSetPointCallback) {
        tempSetPointCallback(temp);
      }
      this->target_temperature = temp;
      this->publish_state();
    }
  }

  climate::ClimateTraits traits() override {
    // The capabilities of the climate device
    auto traits = climate::ClimateTraits();
    traits.set_supports_current_temperature(true);
    traits.set_supported_modes({climate::CLIMATE_MODE_HEAT});
    traits.set_visual_temperature_step(1);
    return traits;
  }
};}