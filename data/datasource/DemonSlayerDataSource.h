#pragma once

#include <string>

#include "http/IHttpClient.h"

namespace data::http
{
class IHttpClient;
}

namespace data::remote
{
class DemonSlayerDataSource
{
  public:
    DemonSlayerDataSource(http::IHttpClient& httpClient);
    ~DemonSlayerDataSource() = default;

    [[nodiscard]] std::string getCharacters() const;

  private:
    http::IHttpClient& m_httpClient;
};
} // namespace data::remote