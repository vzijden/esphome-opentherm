#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/opentherm/OpenThermListener.h"
#include "esphome/components/opentherm/OpenThermMaster.h"
namespace esphome::opentherm {

enum OpenThermSensorType { BOILER_TEMPERATURE, BOILER_RETURN_TEMPERATURE, BOILER_RELATIVE_MODULATION_LEVEL,
  BOILER_TARGET_TEMPERATURE
};

class OpenThermSensor : public sensor::Sensor, public PollingComponent {
 public:
  void setup() override;
  void dump_config() override;

  void set_sensor_type(OpenThermSensorType sensor_type) { OpenThermSensor::sensor_type_ = sensor_type; }
  OpenThermSensor();

 private:
  OpenThermSensorType sensor_type_;
  OpenThermMaster *open_therm_master_;

 public:
  void set_open_therm_master(OpenThermMaster *open_therm_master) {
    OpenThermSensor::open_therm_master_ = open_therm_master;
  }
  void update() override;

 private:
  float read_from_boiler(OpenThermMessageID message_id);
};

}  // namespace esphome::opentherm
