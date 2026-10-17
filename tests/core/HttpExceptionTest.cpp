#include "data/http/HttpException.h"

int main()
{
  const data::http::HttpException responseError("not found", 404);
  if (responseError.statusCode() != 404 || std::string_view(responseError.what()) != "not found") {
    return 1;
  }

  const data::http::HttpException transportError("connection reset");
  if (transportError.statusCode() != 0 || std::string_view(transportError.what()) != "connection reset") {
    return 1;
  }

  return 0;
}
