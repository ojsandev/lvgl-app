#pragma once

#include "ICertificateStore.h"

namespace infra::tls {
class LinuxCertificateStore : public ICertificateStore {
public:
  void configure(boost::asio::ssl::context& context) override;
};
} // namespace infra::tls
