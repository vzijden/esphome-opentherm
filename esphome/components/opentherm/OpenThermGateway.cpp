
#include "OpenThermGateway.h"
#include "esphome/core/log.h"

namespace esphome::opentherm {

uint32_t OpenThermGateway::proxy_thermostat_request_to_boiler(uint32_t request) {
  if (gateway_mode_) {
    return send_request_to_boiler(request);
  } else {
    return OpenTherm::buildResponse(UNKNOWN_DATA_ID, OpenTherm::getDataID(request), 0);
  }
}

void OpenThermGateway::setup() {
  ESP_LOGCONFIG("opentherm.gateway", "initializing..");

  if (master_in_pin_ && master_out_pin_) {
    open_therm_cv_ = new OpenThermMaster(master_in_pin_, master_out_pin_);
  }

  if (slave_in_pin_ && slave_out_pin_) {
    open_therm_thermostat_ = new OpenThermSlave(slave_in_pin_, slave_out_pin_);

    open_therm_thermostat_->set_on_thermostat_request_callback(
        [this](uint32_t request) { return proxy_thermostat_request_to_boiler(request); });
  }
}

void OpenThermGateway::set_boiler_target_temperature(float temperature) {
  if (gateway_mode_) {
    ESP_LOGCONFIG("opentherm.gateway",
                  "Received request for setting boiler target temperature, ignoring because gateway mode is activated");
  } else {
    ESP_LOGCONFIG("opentherm.gateway", "Setting boiler target temperature to %f", temperature);
    open_therm_cv_->set_target_temperature(temperature);
  }
}

void OpenThermGateway::set_heating_on(bool heating_on) {
  this->heating_on_ = heating_on;
  update_boiler_status_();
}

void OpenThermGateway::set_hot_water(bool hot_water_on) {
  this->domestic_hot_water_on_ = hot_water_on;
  update_boiler_status_();
}

void OpenThermGateway::dump_config() {
  ESP_LOGCONFIG("opentherm.gateway", "Master in pin %d", master_in_pin_);
  ESP_LOGCONFIG("opentherm.gateway", "Master out pin %d", master_out_pin_);
  ESP_LOGCONFIG("opentherm.gateway", "Slave in pin %d", slave_in_pin_);
  ESP_LOGCONFIG("opentherm.gateway", "Slave out pin %d", slave_out_pin_);
}

void OpenThermGateway::register_sensor(OpenThermListener *sensor) { sensors_.push_back(sensor); }
void OpenThermGateway::update() {}
OpenThermGateway::OpenThermGateway() : PollingComponent(3000) {}
void OpenThermGateway::loop() { open_therm_thermostat_->loop(); }

void OpenThermGateway::update_boiler_status_() const {
  if (gateway_mode_) {
    ESP_LOGD("opentherm.gateway", "Ignoring write to boiler status because gateway mode is on");
  } else {
    ESP_LOGD("opentherm.master", "Updating boiler status to heating: %d, hot_water:%d", heating_on_,
             domestic_hot_water_on_);
    auto request = OpenTherm::buildSetBoilerStatusRequest(heating_on_, domestic_hot_water_on_);
    OpenThermGateway::send_request_to_boiler(request);
  }
}

uint32_t OpenThermGateway::send_write_to_boiler(OpenThermMessageID messageId, uint32_t data) {
  if (gateway_mode_) {
    ESP_LOGV("opentherm.gateway", "Ignoring write to boiler because gateway mode is on: %s",
             OpenTherm::messageIdToString(messageId));
    return -1;
  } else {
    return send_request_to_boiler(messageId);
  }
}

unsigned long OpenThermGateway::send_request_to_boiler(unsigned long request) const {
  ESP_LOGD("opentherm.master", "Sending request to boiler: %s",
           OpenTherm::messageIdToString(OpenTherm::getDataID(request)));
  auto response = open_therm_cv_->send_request_to_boiler_(request);
  for (const auto &sensor : sensors_) {
    sensor->on_response(request, response);
  }
  return response;
}

uint32_t OpenThermGateway::read_from_boiler(OpenThermMessageID message_id) {
  return send_request_to_boiler(OpenTherm::buildRequest(READ, message_id, 0));
}

}  // namespace esphome::opentherm