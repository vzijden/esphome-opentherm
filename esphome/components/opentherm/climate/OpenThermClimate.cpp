//
// Created by Vincent van der Zijden on 18/01/2023.
//

#include "OpenThermClimate.h"

static const char * TAG = "opentherm.climate";
void esphome::opentherm::OpenThermClimate::control(const esphome::climate::ClimateCall &call) {
  if (call.get_target_temperature().has_value()) {
    open_therm_gateway_->set_temperature_setpoint_override(call.get_target_temperature().value());
    this->target_temperature = call.get_target_temperature().value();

  }
  this->publish_state();
}
void esphome::opentherm::OpenThermClimate::on_response(uint32_t request, uint32_t response) {
  if (OpenTherm::getDataID(request) == Status) {
    if (OpenTherm::isCentralHeatingActive(response)) {
      ESP_LOGD(TAG, "Intercepted status request, setting HEATING");
      this->action = climate::CLIMATE_ACTION_HEATING;
    } else {
      ESP_LOGD(TAG, "Intercepted status request, setting IDLE");
      this->action = climate::CLIMATE_ACTION_IDLE;
    }
    this->publish_state();
  }

  if (OpenTherm::getDataID(request) == Tr) {
    ESP_LOGD(TAG, "intercepted request for room temperature");
    float temp = OpenTherm::getFloat(response);
    this->current_temperature = temp;
    this->publish_state();
  }

  if (OpenTherm::getDataID(request) == TrSet) {
    ESP_LOGD(TAG, "intercepted request for setpoint");
    float temp = OpenTherm::getFloat(response);
    this->target_temperature = temp;
    this->publish_state();
  }
}
void esphome::opentherm::OpenThermClimate::setup() {
  ESP_LOGD(TAG, "Setup");
  this->target_temperature = 18;
  this->mode = climate::CLIMATE_MODE_HEAT;
  this->publish_state();

  open_therm_gateway_->add_listener(this);
}
void esphome::opentherm::OpenThermClimate::update() {
}
esphome::opentherm::OpenThermClimate::OpenThermClimate() : PollingComponent(30000) {}
