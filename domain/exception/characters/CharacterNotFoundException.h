#pragma once

#include "CharacterException.h"

namespace domain::exception {

class CharacterNotFoundException : public CharacterException {
public:
  CharacterNotFoundException() = default;
  ~CharacterNotFoundException() override = default;
};

} // namespace domain::exception
