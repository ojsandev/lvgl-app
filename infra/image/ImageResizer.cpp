#include "ImageResizer.h"

#include <algorithm>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <webp/decode.h>
#include <webp/encode.h>

#include "common/types.h"

namespace {

struct AlphaBounds
{
  int left;
  int top;
  int right;
  int bottom;
};

std::optional<AlphaBounds> findAlphaBounds(const std::uint8_t* rgba, const int width, const int height)
{
  int left = width;
  int top = height;
  int right = -1;
  int bottom = -1;

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const auto alpha =
          rgba[(static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)) * 4U + 3U];

      if (alpha == 0) {
        continue;
      }

      left = std::min(left, x);
      top = std::min(top, y);
      right = std::max(right, x);
      bottom = std::max(bottom, y);
    }
  }

  if (right < left || bottom < top) {
    return std::nullopt;
  }

  return AlphaBounds{
    .left = left,
    .top = top,
    .right = right,
    .bottom = bottom,
  };
}

struct WebPDeleter
{
  void operator()(std::uint8_t* data) const noexcept
  {
    if (data != nullptr) {
      WebPFree(data);
    }
  }
};

using WebPDataPtr = std::unique_ptr<std::uint8_t[], WebPDeleter>;

std::vector<std::uint8_t> readFile(const std::string& path)
{
  std::ifstream file(path, std::ios::binary | std::ios::ate);

  if (!file) {
    return {};
  }

  const std::streamsize size = file.tellg();

  if (size <= 0) {
    return {};
  }

  std::vector<std::uint8_t> data(static_cast<std::size_t>(size));

  file.seekg(0);

  if (!file.read(reinterpret_cast<char*>(data.data()), size)) {
    return {};
  }

  return data;
}

bool writeFile(const std::string& path, const std::uint8_t* data, const std::size_t size)
{
  std::ofstream file(path, std::ios::binary | std::ios::trunc);

  if (!file) {
    return false;
  }

  file.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));

  return file.good();
}

void resizeNearestNeighbor(const std::uint8_t* source, const int sourceWidth, const int sourceHeight,
                           std::uint8_t* destination, const int destinationWidth, const int destinationHeight)
{
  for (int y = 0; y < destinationHeight; ++y) {
    const int sourceY =
        std::min(sourceHeight - 1, static_cast<int>((static_cast<std::int64_t>(y) * sourceHeight) / destinationHeight));

    for (int x = 0; x < destinationWidth; ++x) {
      const int sourceX =
          std::min(sourceWidth - 1, static_cast<int>((static_cast<std::int64_t>(x) * sourceWidth) / destinationWidth));

      const std::size_t sourceIndex = (static_cast<std::size_t>(sourceY) * static_cast<std::size_t>(sourceWidth) +
                                       static_cast<std::size_t>(sourceX)) *
                                      4U;
      const std::size_t destinationIndex =
          (static_cast<std::size_t>(y) * static_cast<std::size_t>(destinationWidth) + static_cast<std::size_t>(x)) * 4U;

      destination[destinationIndex + 0U] = source[sourceIndex + 0U];
      destination[destinationIndex + 1U] = source[sourceIndex + 1U];
      destination[destinationIndex + 2U] = source[sourceIndex + 2U];
      destination[destinationIndex + 3U] = source[sourceIndex + 3U];
    }
  }
}

} // namespace

bool infra::image::ImageResizer::createThumbnail(const std::string& inputPath, const std::string& outputPath,
                                                 common::types::UInt32 width, common::types::UInt32 height)
{
  if (width == 0 || height == 0) {
    return false;
  }

  const auto encoded = readFile(inputPath);

  if (encoded.empty()) {
    return false;
  }

  int decodedWidth = 0;
  int decodedHeight = 0;

  WebPDataPtr decoded(WebPDecodeRGBA(encoded.data(), encoded.size(), &decodedWidth, &decodedHeight));

  if (!decoded) {
    return false;
  }

  if (decodedWidth <= 0 || decodedHeight <= 0) {
    return false;
  }

  const auto alphaBounds = findAlphaBounds(decoded.get(), decodedWidth, decodedHeight);

  if (!alphaBounds) {
    return false;
  }

  const int cropWidth = alphaBounds->right - alphaBounds->left + 1;
  const int cropHeight = alphaBounds->bottom - alphaBounds->top + 1;

  std::vector<std::uint8_t> cropped(static_cast<std::size_t>(cropWidth) * static_cast<std::size_t>(cropHeight) * 4U);

  for (int y = 0; y < cropHeight; ++y) {
    const auto* sourceRow =
        decoded.get() + ((static_cast<std::size_t>(alphaBounds->top + y) * static_cast<std::size_t>(decodedWidth)) +
                         static_cast<std::size_t>(alphaBounds->left)) *
                            4U;
    auto* destinationRow = cropped.data() + static_cast<std::size_t>(y) * static_cast<std::size_t>(cropWidth) * 4U;

    std::copy_n(sourceRow, static_cast<std::size_t>(cropWidth) * 4U, destinationRow);
  }

  const double scaleX = static_cast<double>(width) / static_cast<double>(cropWidth);
  const double scaleY = static_cast<double>(height) / static_cast<double>(cropHeight);
  const double scale = std::min(scaleX, scaleY);

  const int resizedWidth = std::max(1, static_cast<int>(static_cast<double>(cropWidth) * scale));
  const int resizedHeight = std::max(1, static_cast<int>(static_cast<double>(cropHeight) * scale));

  std::vector<std::uint8_t> resized(static_cast<std::size_t>(resizedWidth) * static_cast<std::size_t>(resizedHeight) *
                                    4U);

  resizeNearestNeighbor(cropped.data(), cropWidth, cropHeight, resized.data(), resizedWidth, resizedHeight);

  std::vector<std::uint8_t> thumbnail(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4U, 0);

  const int offsetX = (static_cast<int>(width) - resizedWidth) / 2;
  const int offsetY = (static_cast<int>(height) - resizedHeight) / 2;

  for (int y = 0; y < resizedHeight; ++y) {
    const auto* sourceRow = resized.data() + static_cast<std::size_t>(y) * static_cast<std::size_t>(resizedWidth) * 4U;
    auto* destinationRow = thumbnail.data() +
                           static_cast<std::size_t>(y + offsetY) * static_cast<std::size_t>(width) * 4U +
                           static_cast<std::size_t>(offsetX) * 4U;

    std::copy_n(sourceRow, static_cast<std::size_t>(resizedWidth) * 4U, destinationRow);
  }

  std::uint8_t* encodedThumbnail = nullptr;

  const auto encodedSize = WebPEncodeLosslessRGBA(thumbnail.data(), static_cast<int>(width), static_cast<int>(height),
                                                  static_cast<int>(width * 4U), &encodedThumbnail);

  if (encodedSize <= 0 || encodedThumbnail == nullptr) {
    return false;
  }

  const WebPDataPtr encodedResult(encodedThumbnail);

  return writeFile(outputPath, encodedResult.get(), encodedSize);
}
