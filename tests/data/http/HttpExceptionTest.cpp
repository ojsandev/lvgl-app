#include <gtest/gtest.h>

#include "data/http/HttpException.h"

TEST(HttpExceptionTest, PreservesMessageAndStatusCode)
{
  const data::http::HttpException error("not found", 404);

  EXPECT_EQ(error.statusCode(), 404);
  EXPECT_STREQ(error.what(), "not found");
}

TEST(HttpExceptionTest, DefaultsStatusCodeForTransportError)
{
  const data::http::HttpException error("connection reset");

  EXPECT_EQ(error.statusCode(), 0);
  EXPECT_STREQ(error.what(), "connection reset");
}
