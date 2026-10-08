#include "LinuxCertificateStore.h"

#include "common/logger/Logging.h"

using namespace infra::tls;

void LinuxCertificateStore::configure(boost::asio::ssl::context& sslContext)
{
  if (const char* caFile = std::getenv("SSL_CERT_FILE"); caFile != nullptr) {
    sslContext.load_verify_file(caFile);
  } else if (const char* caPath = std::getenv("SSL_CERT_DIR"); caPath != nullptr) {
    sslContext.add_verify_path(caPath);
  } else {
    // Default paths for Linux
    sslContext.add_verify_path("/etc/ssl/certs");
    sslContext.add_verify_path("/usr/local/share/ca-certificates");
  }

  LOG_INFO("Certificate store configured");
}
