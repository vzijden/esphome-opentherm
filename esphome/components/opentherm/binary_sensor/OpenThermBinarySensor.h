#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/opentherm/OpenThermListener.h"
#include "esphome/components/opentherm/OpenThermMaster.h"
namespace esphome::opentherm {

enum OpenThermBinarySensorType { FLAME_ON };

class OpenThermBinarySensor : public binary_sensor::BinarySensor, public PollingComponent {
 public:
  void dump_config() override;

  void set_sensor_type(OpenThermBinarySensorType sensor_type) { OpenThermBinarySensor::sensor_type_ = sensor_type; }

 private:
  OpenThermBinarySensorType sensor_type_;
  OpenThermMaster *open_therm_master_;

 public:
  void set_open_therm_master(OpenThermMaster *open_therm_master) {
    OpenThermBinarySensor::open_therm_master_ = open_therm_master;
  }
  void update() override;
  explicit OpenThermBinarySensor();
};

}  // namespace esphome::opentherm
