#include "CertificateStoreFactory.h"

#ifdef __APPLE__
#include "MacOSCertificateStore.h"
#elif defined(__linux__)
#include "LinuxCertificateStore.h"
#elif defined(_WIN32)
#include "WindowsCertificateStore.h"
#endif


std::unique_ptr<infra::tls::ICertificateStore> infra::tls::createCertificateStore()
{
#ifdef __APPLE__

    return std::make_unique<MacOSCertificateStore>();

#elif defined(__linux__)

    return std::make_unique<LinuxCertificateStore>();

#elif defined(_WIN32)

    return std::make_unique<WindowsCertificateStore>();

#else

#error "Unsupported platform"

#endif
}