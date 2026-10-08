#include "CharacterMapper.h"

#include <boost/json.hpp>

#include "domain/model/CharactersResponse.h"

using namespace data::mapper;

domain::model::CharactersResponse CharacterMapper::fromJson(const std::string_view& json)
{
  const auto value = boost::json::parse(json);

  const auto& root = value.as_object();
  const auto& characters = root.at("content").as_array();
  const auto& paginationObject = root.at("pagination").as_object();

  domain::model::Pagination pagination;

  pagination.totalElements = paginationObject.at("totalElements").as_int64();

  pagination.elementsOnPage = paginationObject.at("elementsOnPage").as_int64();

  pagination.currentPage = paginationObject.at("currentPage").as_int64();

  if (const auto totalPages = paginationObject.if_contains("totalPages");
      totalPages != nullptr && !totalPages->is_null()) {
    pagination.totalPages = totalPages->as_int64();
  } else {
    pagination.totalPages = 0; // or some default value
  }

  if (const auto previousPage = paginationObject.if_contains("previousPage");
      previousPage != nullptr && !previousPage->is_null()) {
    pagination.previousPage = previousPage->as_string();
  }

  if (const auto nextPage = paginationObject.if_contains("nextPage"); nextPage != nullptr && !nextPage->is_null()) {
    pagination.nextPageUrl = nextPage->as_string();
  }

  domain::model::CharactersResponse response;
  response.pagination = pagination;

  for (const auto& item : characters) {
    const auto& object = item.as_object();
    domain::model::Character character;

    character.info.id = object.at("id").as_int64();
    character.info.name = object.at("name").as_string();

    if (const auto age = object.if_contains("age"); age != nullptr && !age->is_null()) {
      character.info.age = age->as_int64();
    }

    character.info.gender = object.at("gender").as_string();
    character.info.race = object.at("race").as_string();
    character.info.description = object.at("description").as_string();
    character.img = object.at("img").as_string();

    response.characters.push_back(std::move(character));
  }

  return response;
}
