#pragma once

#include <string>

#include "common/types.h"

namespace domain::model {
struct Pagination
{
  common::types::UInt64 totalElements;
  common::types::UInt64 elementsOnPage;
  common::types::UInt64 currentPage;
  common::types::UInt64 totalPages;
  std::string previousPage;
  std::string nextPageUrl;
  auto operator<=>(const Pagination&) const = default;
};
} // namespace domain::model
