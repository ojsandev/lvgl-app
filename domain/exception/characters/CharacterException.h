#pragma once
#include <exception>

namespace domain::exception {
class CharacterException : public std::exception {
public:
  CharacterException() = default;
  ~CharacterException() override = default;
};
} // namespace domain::exception
