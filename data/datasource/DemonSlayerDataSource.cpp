#include "DemonSlayerDataSource.h"

using namespace data::remote;

DemonSlayerDataSource::DemonSlayerDataSource(http::IHttpClient& httpClient)
    : m_httpClient(httpClient)
{
}

std::string DemonSlayerDataSource::getCharacters() const
{
    return m_httpClient.get("https://demonslayerapi.com/api/v1/characters");
}