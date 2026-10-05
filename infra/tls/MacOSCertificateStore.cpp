#include "MacOSCertificateStore.h"

#include <filesystem>
#include <print>
#include <stdexcept>

using namespace infra::tls;

void MacOSCertificateStore::configure(boost::asio::ssl::context& context)
{
  constexpr std::string_view caBundle = "/etc/ssl/cert.pem";

  if (!std::filesystem::exists(caBundle)) {
    throw std::runtime_error("MacOSCertificateStore::configure: macOS CA bundle not found: " + std::string(caBundle));
  }

  std::print("MacOSCertificateStore::configure: loading CA bundle: {}\n", caBundle);

  context.load_verify_file(std::string(caBundle));

  context.set_verify_mode(boost::asio::ssl::verify_peer);

  std::print("MacOSCertificateStore::configure: CA bundle loaded successfully\n");
}
