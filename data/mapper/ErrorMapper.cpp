#include "ErrorMapper.h"

using namespace data::mapper;

domain::model::error::Error ErrorMapper::map(const std::string& message, const int code)
{
  switch (code) {
  case 404:
    return domain::model::error::Error{.reason = domain::model::error::Reason::NotFound, .message = message};
  case 400:
    return domain::model::error::Error{.reason = domain::model::error::Reason::InvalidInput, .message = message};
  case 401:
    return domain::model::error::Error{.reason = domain::model::error::Reason::Unauthorized, .message = message};
  case 403:
    return domain::model::error::Error{.reason = domain::model::error::Reason::Forbidden, .message = message};
  case 409:
    return domain::model::error::Error{.reason = domain::model::error::Reason::Conflict, .message = message};
  case 500:
    return domain::model::error::Error{.reason = domain::model::error::Reason::InternalServerError, .message = message};
  default:
    return domain::model::error::Error{.reason = domain::model::error::Reason::Unknown, .message = message};
  }
}
