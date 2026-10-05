#include <print>
#include <string>

#include "http/BoostBeastHttpClient.h"

using namespace infra::http;

int main()
{
  try {
    std::print("=== TLS Test ===\n");

    BoostBeastHttpClient client;

    const auto response = client.get("https://www.demonslayer-api.com/api/v1/characters?page=1&limit=1");

    std::print("HTTPS request succeeded!\n\n"
               "Response:\n{}\n",
               response);

    return 0;
  }
  catch (const std::exception& exception) {
    std::print("TLS test failed:\n{}\n", exception.what());
    return 1;
  }
}
