#pragma once

#include <cstdint>

namespace esphome::opentherm {
class OpenThermListener {
 public:
  virtual ~OpenThermListener() = default;
  virtual void on_response(uint32_t request, uint32_t response);
};
}  // namespace esphome::opentherm