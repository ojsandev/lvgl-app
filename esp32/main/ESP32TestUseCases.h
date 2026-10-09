#pragma once

#include <memory>
#include <string>

#include "domain/usecase/IUpdateTitleUseCase.h"
#include "domain/usecase/characters/IGetCharactersUseCase.h"

namespace esp32::test {

class UpdateTitleUseCase final : public domain::usecase::IUpdateTitleUseCase
{
public:
  std::string execute() override
  {
    return "Demon Slayer - ESP32";
  }
};

class GetCharactersUseCase final : public domain::usecase::IGetCharactersUseCase
{
public:
  domain::model::CharacterView execute(const common::types::Int32 limit, const common::types::Int32 page) override
  {
    return createPage(page);
  }

  domain::model::CharacterView nextPage() override
  {
    return createPage(2);
  }

  domain::model::CharacterView previousPage() override
  {
    return createPage(1);
  }

private:
  static domain::model::CharacterView createPage(common::types::Int32 page)
  {
    domain::model::CharacterView result = {};

    result.pagination.currentPage = page;
    result.pagination.totalPages = 2;

    domain::model::CharacterWithPath character;
    character.info.id = page;
    character.info.name = page == 1 ? "Tanjiro Kamado" : "Nezuko Kamado";
    character.info.age = 15;
    character.info.gender = page == 1 ? "Male" : "Female";
    character.info.race = page == 1 ? "Human" : "Demon";
    character.info.description = "ESP32 interface test character";

    result.characters.push_back(std::move(character));

    return result;
  }
};

} // namespace esp32::test
