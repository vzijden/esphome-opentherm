#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/opentherm/OpenThermListener.h"
#include "../OpenTherm.h"
#include "../OpenThermGateway.h"
namespace esphome::opentherm {

enum OpenThermSensorType { BOILER_TEMPERATURE, BOILER_RETURN_TEMPERATURE, BOILER_RELATIVE_MODULATION_LEVEL,
  BOILER_TARGET_TEMPERATURE, MAX_RELATIVE_MODULATION_LEVEL
};

class OpenThermSensor : public sensor::Sensor, public PollingComponent, public OpenThermListener {
 public:
  void setup() override;
  void dump_config() override;

  void set_sensor_type(OpenThermSensorType sensor_type) { OpenThermSensor::sensor_type_ = sensor_type; }
  OpenThermSensor();

 private:
  OpenThermSensorType sensor_type_;
  OpenThermGateway *open_therm_gateway_;
  bool is_message_for_sensor_type(OpenThermMessageID message_id, OpenThermMessageType response_type);
  uint32_t send_request_to_boiler(OpenThermMessageID message_id);

 public:
  void set_open_therm_gateway(OpenThermGateway *open_therm_gateway) {
    OpenThermSensor::open_therm_gateway_ = open_therm_gateway;
  }
  void update() override;
  void on_response(unsigned long request, unsigned long response) override;
};

}  // namespace esphome::opentherm
