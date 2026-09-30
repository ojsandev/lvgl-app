#pragma once

#include "ICertificateStore.h"

namespace infra::tls
{
class MacOSCertificateStore : public ICertificateStore
{
public:
    void configure(boost::asio::ssl::context& context) override;
};
}