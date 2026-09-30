#pragma once

#include <boost/asio/ssl/context.hpp>

namespace infra::tls
{
class ICertificateStore
{
public:
    virtual ~ICertificateStore() = default;

    virtual void configure(boost::asio::ssl::context& sslContext) = 0;
};
}