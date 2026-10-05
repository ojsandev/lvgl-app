#include "ErrorMapper.h"

using namespace data::mapper;
using namespace domain::model;

error::Error ErrorMapper::map(const std::string& message, const int code)
{
  switch (code) {
  case 404:
    return error::Error{.reason = error::Reason::NotFound, .message = message};
  case 400:
    return error::Error{.reason = error::Reason::InvalidInput, .message = message};
  case 401:
    return error::Error{.reason = error::Reason::Unauthorized, .message = message};
  case 403:
    return error::Error{.reason = error::Reason::Forbidden, .message = message};
  case 409:
    return error::Error{.reason = error::Reason::Conflict, .message = message};
  case 500:
    return error::Error{.reason = error::Reason::InternalServerError, .message = message};
  default:
    return error::Error{.reason = error::Reason::Unknown, .message = message};
  }
}
