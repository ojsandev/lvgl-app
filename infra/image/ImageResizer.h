#pragma once

#include <cstdint>
#include <string>

#include "domain/repository/IImageResizerRepository.h"

namespace infra::image {

class ImageResizer : public domain::repository::IImageResizerRepository {
public:
  bool createThumbnail(const std::string& inputPath, const std::string& outputPath, common::types::UInt32 width,
                       common::types::UInt32 height) override;
};

} // namespace infra::image
