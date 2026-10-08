#pragma once

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace common::uuid {
inline std::string generate()
{
  boost::uuids::random_generator gen;
  return boost::uuids::to_string(gen());
}
} // namespace common::uuid
