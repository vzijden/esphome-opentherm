#include "OpenTherm.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/core/component.h"
#include "esphome/components/api/api_server.h"
#include "OpenThermClimate.h"

// Pins to OpenTherm Adapter
const int M_IN_PIN = D6;
const int M_OUT_PIN = D5;

const int S_IN_PIN = D2;
const int S_OUT_PIN = D1;

OpenTherm mOT(M_IN_PIN, M_OUT_PIN);
OpenTherm sOT(S_IN_PIN, S_OUT_PIN, true);

void IRAM_ATTR mHandleInterrupt() { mOT.handleInterrupt(); }

void IRAM_ATTR sHandleInterrupt() { sOT.handleInterrupt(); }

namespace esphome {
namespace opentherm {

class OpenThermGateway : public PollingComponent {
 private:
  const char *TAG = "OpenThermGateway";

 public:
  sensor::Sensor *boiler_target_temp_sensor = new sensor::Sensor();
  sensor::Sensor *boiler_temp_sensor = new sensor::Sensor();
  sensor::Sensor *thermostat_room_temperature = new sensor::Sensor();
  sensor::Sensor *thermostat_room_setpoint = new sensor::Sensor();
  sensor::Sensor *max_relative_modulation_setting = new sensor::Sensor();
  binary_sensor::BinarySensor *flame = new binary_sensor::BinarySensor();
  binary_sensor::BinarySensor *CH = new binary_sensor::BinarySensor();
  OpenThermClimate *climate = new OpenThermClimate();

  OpenThermGateway() : PollingComponent(3000) {
  }

  void processRequest(unsigned long request, OpenThermResponseStatus status) {
    unsigned long data_id = sOT.getDataID(request);
    if (sOT.getDataID(request) == Toutside) {
      ESP_LOGD("opentherm_component", "Overriding response for Toutside");
      sOT.sendResponse(sOT.buildResponse(READ_ACK, Toutside, sOT.temperatureToData(8)));
      return;
    }

    if (sOT.getDataID(request) == TrOverride) {
      float setpoint = climate->target_temperature;

      ESP_LOGD("opentherm_component", "Responding to trOverride with %f", setpoint);
      sOT.sendResponse(sOT.buildResponse(READ_ACK, TrOverride, sOT.temperatureToData(setpoint)));
      return;
    }

    unsigned long response = mOT.sendRequest(request);

    if (response) {
      sOT.sendResponse(response);

      if (data_id == Status) {
        flame->publish_state(mOT.isFlameOn(response));
        CH->publish_state(mOT.isCentralHeatingActive(response));
      }

      if (data_id == TSet) {
        boiler_target_temp_sensor->publish_state(mOT.getFloat(response));
      }

      if (data_id == Tboiler) {
        boiler_temp_sensor->publish_state(mOT.getFloat(response));
      }

      if (data_id == TrSet) {
        thermostat_room_setpoint->publish_state(mOT.getFloat(response));
      }

      if (data_id == Tr) {
        thermostat_room_temperature->publish_state(mOT.getFloat(response));
      }

      if (data_id == RelModLevel) {
        max_relative_modulation_setting->publish_state(mOT.getFloat(response));
      }
    }
  }

  void onRoomSetPointChange(int setpoint) {
    ESP_LOGD("opentherm_component", "Changing setpoint to %d", setpoint);
    sOT.sendResponse(sOT.buildResponse(READ_ACK, TrOverride, sOT.temperatureToData(setpoint)));

    //        unsigned int data = 0;
    //        data <<= 8;
    //        sOT.sendResponse(sOT.sendResponse(READ_ACK, RemoteOverrideFunction, data));
  }

  void setup() override {
    Serial.begin(9600);  // 9600 supported by OpenTherm Monitor App
    mOT.begin(mHandleInterrupt);

    std::function<void(long unsigned int, OpenThermResponseStatus)> callback =
        std::bind(&OpenThermGateway::processRequest, this, std::placeholders::_1, std::placeholders::_2);
    sOT.begin(sHandleInterrupt, callback);

    climate->registerTempSetpointCallback(std::bind(&OpenThermGateway::onRoomSetPointChange, this, std::placeholders::_1));
  }

  void update() override {}

  void loop() override { sOT.process(); }
};

}  // namespace opentherm
}  // namespace esphome