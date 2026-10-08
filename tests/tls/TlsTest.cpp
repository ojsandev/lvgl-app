#include <fstream>
#include <ios>
#include <print>
#include <string>

#include "common/logger/Logging.h"
#include "http/BoostBeastHttpClient.h"

using namespace infra::http;

int main()
{
  try {
    LOG_INFO("=== TLS Test ===\n");

    BoostBeastHttpClient client;

    const auto response = client.get("https://www.demonslayer-api.com/api/v1/characters?page=1&limit=1");

    LOG_INFO("HTTPS request succeeded!\n" "Response: \n{}", response);

    return 0;
  } catch (const std::exception& exception) {
    LOG_ERROR("TLS test failed: {}\n", exception.what());
    return 1;
  }
}
