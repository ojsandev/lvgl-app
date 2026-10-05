#pragma once
#include "CharacterException.h"

namespace domain::exception {

class CharacterInvalidArgsException : public CharacterException {
public:
  CharacterInvalidArgsException() = default;
  ~CharacterInvalidArgsException() override = default;
};

} // namespace domain::exception
