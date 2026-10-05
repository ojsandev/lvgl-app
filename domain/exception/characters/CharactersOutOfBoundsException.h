#pragma once

#include "CharacterException.h"

namespace domain::exception {
class CharactersOutOfBoundsException : public CharacterException {
public:
  CharactersOutOfBoundsException() = default;
  ~CharactersOutOfBoundsException() override = default;
};
} // namespace domain::exception
