#pragma once

#include <optional>
#include <string>

#include "common/types.h"

namespace domain::model {
struct Character {
  common::types::Int64 id;
  std::string name;
  std::optional<common::types::Int64> age;
  std::string gender;
  std::string race;
  std::string description;
  std::string img;
};
} // namespace domain::model
