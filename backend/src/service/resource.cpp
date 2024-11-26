#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include "drp/service/resource.hpp"
#include "drp/messages/message.hpp"
#include "drp/service/exceptions.hpp"

using namespace DRP::Service;

/// Destructor
IResource::~IResource() = default;

/// Process a request
std::unique_ptr<DRP::Messages::IMessage>
    IResource::processRequest(const std::string &request)
{
    auto object = nlohmann::json::parse(request);
    return processRequest(object);
}

std::unique_ptr<DRP::Messages::IMessage>
    IResource::operator()(const std::string &request)
{
    return processRequest(request);
}

std::unique_ptr<DRP::Messages::IMessage>
    IResource::operator()(const nlohmann::json &object)
{
    return processRequest(object);
}


/// Gets the documentation
std::string IResource::getDocumentation() const noexcept
{
   return "";
}
