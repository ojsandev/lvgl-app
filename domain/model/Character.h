#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Pagination.h"
#include "common/types.h"

namespace domain::model {
struct CharacterInfo
{
  common::types::Int32 id;
  std::string name;
  std::optional<common::types::Int32> age;
  std::string gender;
  std::string race;
  std::string description;
};

struct Character
{
  CharacterInfo info;
  std::string img;
};

struct CharacterWithPath
{
  CharacterInfo info;
  std::string imagePath;
  std::string thumbnailPath;
};

struct CharacterView
{
  std::vector<CharacterWithPath> characters;
  Pagination pagination;
};
} // namespace domain::model
