#pragma once

#include <memory>
#include <string>
#include <utility>

#include "common/async/Task.h"
#include "domain/usecase/IUpdateTitleUseCase.h"
#include "domain/usecase/characters/IGetCharactersUseCase.h"

namespace esp32::demo {

class UpdateTitleUseCase final : public domain::usecase::IUpdateTitleUseCase {
public:
  std::string execute() override
  {
    return "Demon Slayer - ESP32";
  }
};

class GetCharactersUseCase final : public domain::usecase::IGetCharactersUseCase {
public:
  Result execute(const common::types::Int32, const common::types::Int32 page) override
  {
    return readyPage(page);
  }

  Result nextPage() override
  {
    return readyPage(2);
  }

  Result previousPage() override
  {
    return readyPage(1);
  }

private:
  static Result readyPage(const common::types::Int32 page)
  {
    common::async::TaskCompletionSource<domain::model::CharacterView> completion;
    auto task = completion.task();
    completion.set_value(createPage(page));
    return task;
  }

  static domain::model::CharacterView createPage(const common::types::Int32 page)
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
    character.info.description = "ESP32 UI demonstration character";
    result.characters.push_back(std::move(character));
    return result;
  }
};

} // namespace esp32::demo
