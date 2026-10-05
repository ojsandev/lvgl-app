#include "MacOSCertificateStore.h"

#include <filesystem>
#include <print>
#include <stdexcept>

#include "common/logger/Logging.h"

using namespace infra::tls;

void MacOSCertificateStore::configure(boost::asio::ssl::context& context)
{
  constexpr std::string_view caBundle = "/etc/ssl/cert.pem";

  if (!std::filesystem::exists(caBundle)) {
    LOG_ERROR("macOS CA bundle not found: {}", caBundle);
    throw std::runtime_error("MacOSCertificateStore::configure: macOS CA bundle not found: " + std::string(caBundle));
  }

  context.load_verify_file(std::string(caBundle));

  LOG_DEBUG("macOS CA bundle loaded: {}", caBundle);

  context.set_verify_mode(boost::asio::ssl::verify_peer);
}
