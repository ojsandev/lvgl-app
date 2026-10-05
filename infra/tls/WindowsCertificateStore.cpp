#include "WindowsCertificateStore.h"

#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <print>
#include <stdexcept>
#include <wincrypt.h>
#include <windows.h>

using namespace infra::tls;

void WindowsCertificateStore::configure(boost::asio::ssl::context& context)
{
  SSL_CTX* sslContext = context.native_handle();

  if (sslContext == nullptr) {
    throw std::runtime_error("Failed to obtain native OpenSSL SSL_CTX");
  }

  X509_STORE* store = SSL_CTX_get_cert_store(sslContext);

  if (store == nullptr) {
    throw std::runtime_error("Failed to obtain OpenSSL certificate store");
  }

  HCERTSTORE certificateStore = CertOpenSystemStoreW(0, L"ROOT");

  if (certificateStore == nullptr) {
    throw std::runtime_error("Failed to open Windows ROOT certificate store");
  }

  int importedCertificates = 0;

  PCCERT_CONTEXT certificate = nullptr;

  while ((certificate = CertEnumCertificatesInStore(certificateStore, certificate)) != nullptr) {
    const unsigned char* encoded = certificate->pbCertEncoded;

    X509* x509 = d2i_X509(nullptr, &encoded, certificate->cbCertEncoded);

    if (x509 == nullptr) {
      continue;
    }

    if (X509_STORE_add_cert(store, x509) == 1) {
      ++importedCertificates;
    }

    X509_free(x509);
  }

  CertCloseStore(certificateStore, 0);

  std::print("WindowsCertificateStore: imported {} certificates\n", importedCertificates);

  if (importedCertificates == 0) {
    throw std::runtime_error("No certificates could be imported from Windows ROOT store");
  }

  context.set_verify_mode(boost::asio::ssl::verify_peer);
}
