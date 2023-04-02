#include "OpenThermSensor.h"
#include "esphome/components/opentherm/OpenTherm.h"

namespace esphome::opentherm {

void OpenThermSensor::setup() {
  ESP_LOGCONFIG("opentherm.sensor", "init");
  open_therm_gateway_->add_listener(this);
}
void OpenThermSensor::dump_config() { ESP_LOGCONFIG("opentherm.sensor", "Sensor type:"); }
void OpenThermSensor::update() {
  if (sensor_type_ == BOILER_RETURN_TEMPERATURE) {
    publish_state(OpenTherm::getFloat(send_request_to_boiler(Tret)));
  }
}
OpenThermSensor::OpenThermSensor() : PollingComponent(30000) {}

void OpenThermSensor::on_response(unsigned long request, unsigned long response) {
  if (is_message_for_sensor_type(OpenTherm::getDataID(request), OpenTherm::getMessageType(response))) {
    publish_state(OpenTherm::getFloat(response));
  }
}

bool OpenThermSensor::is_message_for_sensor_type(OpenThermMessageID message_id, OpenThermMessageType response_type) {
  switch (sensor_type_) {
    case BOILER_TEMPERATURE:
      return (message_id == Tboiler && response_type == READ_ACK);
    case BOILER_RELATIVE_MODULATION_LEVEL:
      return message_id == RelModLevel && response_type == READ_ACK;
    case BOILER_TARGET_TEMPERATURE:
      return message_id == TSet && response_type == WRITE_ACK;
    case MAX_RELATIVE_MODULATION_LEVEL:
      return message_id == MaxRelModLevelSetting && response_type == WRITE_ACK;
  }

  return false;
}
uint32_t OpenThermSensor::send_request_to_boiler(OpenThermMessageID message_id) {
  return open_therm_gateway_->send_request_to_boiler(OpenTherm::buildRequest(READ, message_id, 0));
}

}  // namespace esphome::opentherm