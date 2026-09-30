#pragma once

#include <memory>

#include "ICertificateStore.h"

namespace infra::tls
{
std::unique_ptr<ICertificateStore>
createCertificateStore();
}