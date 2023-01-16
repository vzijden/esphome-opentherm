#pragma once
#include "esphome/components/output/binary_output.h"
#include "esphome/components/opentherm/OpenThermMaster.h"
#include "esphome/components/opentherm/OpenThermListener.h"
namespace esphome::opentherm {

enum OpenThermBinaryOutputType {
  HEATING,
  HOT_WATER,
};

class OpenThermBinaryOutput : public output::BinaryOutput, public Component, public OpenThermListener {
 private:
  OpenThermMaster *open_therm_master_;
  OpenThermBinaryOutputType open_therm_binary_output_type_;

 public:
  void set_open_therm_master(OpenThermMaster *open_therm_master) {
    OpenThermBinaryOutput::open_therm_master_ = open_therm_master;
  }
  void set_type(OpenThermBinaryOutputType open_therm_binary_output_type) {
    OpenThermBinaryOutput::open_therm_binary_output_type_ = open_therm_binary_output_type;
  }

  void on_response(uint32_t request, uint32_t response) override;

 protected:
  void write_state(bool state) override;

 private:
  bool match_message_type(uint32_t messageId);
};

}  // namespace esphome::opentherm
