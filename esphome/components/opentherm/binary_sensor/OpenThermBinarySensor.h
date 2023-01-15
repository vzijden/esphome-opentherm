#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/opentherm/OpenThermListener.h"
#include "esphome/components/opentherm/OpenThermGateway.h"
namespace esphome::opentherm {

enum OpenThermBinarySensorType { FLAME_ON, HEATING_ON };

class OpenThermBinarySensor : public binary_sensor::BinarySensor, public Component, public OpenThermListener {
 public:
  void dump_config() override;

  void on_response(uint32_t request, uint32_t response) override;
  void set_sensor_type(OpenThermBinarySensorType sensor_type) { OpenThermBinarySensor::sensor_type_ = sensor_type; }

 private:
  OpenThermBinarySensorType sensor_type_;
  OpenThermGateway *open_therm_gateway_;

 public:
  void set_open_therm_gateway(OpenThermGateway *open_therm_gateway) {
    OpenThermBinarySensor::open_therm_gateway_ = open_therm_gateway;
  }
};

}  // namespace esphome::opentherm
