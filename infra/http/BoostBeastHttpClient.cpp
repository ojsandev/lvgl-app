#include "BoostBeastHttpClient.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/url.hpp>
#include <openssl/ssl.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

#include "common/logger/Logging.h"
#include "exception/HttpException.h"
#include "tls/CertificateStoreFactory.h"

using namespace infra::http;

namespace beast = boost::beast;

std::string BoostBeastHttpClient::get(const std::string_view url)
{
  return request(Method::Get, url);
}

std::string BoostBeastHttpClient::post(const std::string_view url, const std::string_view body,
                                       const std::string_view contentType)
{
  return request(Method::Post, url, body, contentType);
}

std::string BoostBeastHttpClient::put(const std::string_view url, const std::string_view body,
                                      const std::string_view contentType)
{
  return request(Method::Put, url, body, contentType);
}

std::string BoostBeastHttpClient::patch(const std::string_view url, const std::string_view body,
                                        const std::string_view contentType)
{
  return request(Method::Patch, url, body, contentType);
}

std::string BoostBeastHttpClient::delete_(const std::string_view url)
{
  return request(Method::Delete, url);
}

std::vector<common::types::UInt8> BoostBeastHttpClient::getBytes(const std::string_view url)
{
  return requestBytes(Method::Get, url);
}

template <typename ResponseBody>
auto BoostBeastHttpClient::requestImpl(const Method method, const std::string_view url, const std::string_view body,
                                       const std::string_view contentType)
{
  const auto parsed = boost::urls::parse_uri_reference(url);

  if (!parsed) {
    throw std::runtime_error("Invalid URL");
  }

  const auto& uri = parsed.value();

  if (uri.scheme() != "https") {
    throw std::runtime_error("Only HTTPS URLs are supported");
  }

  const std::string host{ uri.host() };
  const std::string port{ uri.port().empty() ? "443" : uri.port() };

  std::string target{ uri.encoded_target() };

  if (target.empty()) {
    target = "/";
  }

  boost::asio::io_context ioContext;

  boost::asio::ssl::context sslContext{ boost::asio::ssl::context::tls_client };

  auto certificateStore = infra::tls::createCertificateStore();

  certificateStore->configure(sslContext);

  sslContext.set_verify_mode(boost::asio::ssl::verify_peer);

  using SslStream = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

  SslStream stream{ ioContext, sslContext };

  stream.set_verify_callback(boost::asio::ssl::host_name_verification(host));

  if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
    throw std::runtime_error("Failed to set TLS SNI");
  }

  boost::asio::ip::tcp::resolver resolver{ ioContext };

  const auto results = resolver.resolve(host, port);

  boost::asio::connect(beast::get_lowest_layer(stream), results);

  stream.handshake(boost::asio::ssl::stream_base::client);

  beast::http::verb httpMethod;

  switch (method) {
  case Method::Get:
    httpMethod = beast::http::verb::get;
    break;

  case Method::Post:
    httpMethod = beast::http::verb::post;
    break;

  case Method::Put:
    httpMethod = beast::http::verb::put;
    break;

  case Method::Patch:
    httpMethod = beast::http::verb::patch;
    break;

  case Method::Delete:
    httpMethod = beast::http::verb::delete_;
    break;
  }

  LOG_INFO("Making {} request to URL: {}", to_string(httpMethod), url);

  beast::http::request<beast::http::string_body> httpRequest{ httpMethod, target, 11 };

  httpRequest.set(beast::http::field::host, host);
  httpRequest.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

  if constexpr (std::is_same_v<ResponseBody, beast::http::string_body>) {
    httpRequest.set(beast::http::field::accept, "application/json");
  } else {
    httpRequest.set(beast::http::field::accept, "image/webp");
  }

  if (!body.empty()) {
    httpRequest.body() = body;
    httpRequest.set(beast::http::field::content_type, contentType);
    httpRequest.prepare_payload();
  }

  beast::http::write(stream, httpRequest);

  beast::flat_buffer buffer;

  beast::http::response<ResponseBody> httpResponse;

  beast::http::read(stream, buffer, httpResponse);

  boost::system::error_code error;

  stream.shutdown(error);

  if (error != std::errc::not_connected && error != boost::asio::ssl::error::stream_truncated && error) {
    throw exception::HttpException{ "Failed to shutdown SSL stream: " + error.message() };
  }

  if (const auto statusCode = httpResponse.result_int(); statusCode < 200 || statusCode >= 300) {
    throw exception::HttpException{ httpResponse.reason(), static_cast<common::types::Int16>(statusCode) };
  }

  return std::move(httpResponse.body());
}

std::string BoostBeastHttpClient::request(const Method method, const std::string_view url, const std::string_view body,
                                          const std::string_view contentType)
{
  return requestImpl<beast::http::string_body>(method, url, body, contentType);
}

std::vector<common::types::UInt8> BoostBeastHttpClient::requestBytes(const Method method, const std::string_view url)
{
  return requestImpl<beast::http::vector_body<common::types::UInt8>>(method, url);
}
