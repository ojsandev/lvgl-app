#pragma once

#include <cstdint>
#include <string>

#include "common/types.h"

namespace domain::repository {

class IImageResizerRepository {
public:
  virtual ~IImageResizerRepository() = default;
  virtual bool createThumbnail(const std::string& inputPath, const std::string& outputPath, common::types::UInt32 width,
                               common::types::UInt32 height) = 0;
};

} // namespace domain::repository
