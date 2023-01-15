#pragma once

#include "esphome/core/component.h"
#include "OpenThermMaster.h"
#include "OpenThermListener.h"
#include "vector"

namespace esphome::opentherm {

class OpenThermGateway : public PollingComponent {
 private:
  OpenThermMaster *open_therm_cv_{nullptr};
  OpenThermSlave *open_therm_thermostat_{nullptr};
  int slave_in_pin_{0};
  int slave_out_pin_{0};
  int master_in_pin_{0};
  int master_out_pin_{0};
  bool gateway_mode_{true};
  std::vector<OpenThermListener *>sensors_{std::vector<OpenThermListener*>()};
  unsigned long send_request_to_boiler(unsigned long request) const;
  bool heating_on_{false};
  bool domestic_hot_water_on_{false};
  void update_boiler_status_() const;

 public:
  explicit OpenThermGateway();
  void setup() override;
  void dump_config() override;
  void set_gateway_mode(bool gateway_mode) { gateway_mode_ = gateway_mode; }
  void set_slave_in_pin(int slave_in_pin) { slave_in_pin_ = slave_in_pin; }
  void set_slave_out_pin(int slave_out_pin) { slave_out_pin_ = slave_out_pin; }
  void set_master_in_pin(int master_in_pin) { master_in_pin_ = master_in_pin; }
  void set_master_out_pin(int master_out_pin) { master_out_pin_ = master_out_pin; }
  void register_sensor(OpenThermListener *sensor);
  void set_boiler_target_temperature(float temperature);
  void set_heating_on(bool heating_on);
  OpenThermMaster *get_open_therm_cv() const { return open_therm_cv_; }
  void update() override;
  uint32_t proxy_thermostat_request_to_boiler(uint32_t request);
  void loop() override;
  uint32_t send_write_to_boiler(OpenThermMessageID messageId, uint32_t data);
  uint32_t read_from_thermostat(OpenThermMessageID message_id);
  uint32_t read_from_boiler(OpenThermMessageID message_id);
  void set_hot_water(bool hot_water_on);
};
}  // namespace esphome::opentherm