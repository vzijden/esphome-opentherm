#pragma once

#include <cstdint>

namespace esphome::opentherm {
class OpenThermListener {
 public:
  virtual ~OpenThermListener() = default;
  virtual void on_response(unsigned long request, unsigned long response);
};
}  // namespace esphome::opentherm