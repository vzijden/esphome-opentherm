#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/opentherm/OpenThermListener.h"
#include "esphome/components/opentherm/OpenThermGateway.h"
namespace esphome::opentherm {

enum OpenThermSensorType { THERMOSTAT_ROOM_TEMPERATURE, BOILER_TEMPERATURE, BOILER_RETURN_TEMPERATURE, BOILER_TARGET_TEMPERATURE };

class OpenThermSensor : public sensor::Sensor, public PollingComponent, public OpenThermListener {
 public:
  void setup() override;
  void dump_config() override;

  void on_response(uint32_t request, uint32_t response) override;
  void set_sensor_type(OpenThermSensorType sensor_type) { OpenThermSensor::sensor_type_ = sensor_type; }
  OpenThermSensor();

 private:
  OpenThermSensorType sensor_type_;
  OpenThermGateway *open_therm_gateway_;

 public:
  void set_open_therm_gateway(OpenThermGateway *open_therm_gateway) {
    OpenThermSensor::open_therm_gateway_ = open_therm_gateway;
  }
  void update() override;
};

}  // namespace esphome::opentherm
