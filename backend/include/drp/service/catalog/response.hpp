#ifndef DRP_SERVICE_CATALOG_RESPONSE_HPP
#define DRP_SERVICE_CATALOG_RESPONSE_HPP
#include <memory>
#include <optional>
#include <drp/messages/message.hpp>
namespace DRP::Service::Catalog
{
/// @class Response "response.hpp" "drp/service/catalog/response.hpp"
/// @brief Defines a response to a catalog request.
/// @copyright Ben Baker (University of Utah) distributed under the MIT license.
class Response : public DRP::Messages::IMessage
{
public:
    /// @brief Constructor.
    Response();
    /// @brief Copy constructor.
    Response(const Response &response);
    /// @brief Move constructor.
    Response(Response &&response) noexcept;

    /// @brief Sets the response data.
    /// @param[in] object   The response data to set.
    void setData(const nlohmann::json &object) noexcept;
    /// @brief Sets the response data.
    /// @param[in,out] object  The response data.  On exit, object's behavior
    ///                        is undefined.
    void setData(nlohmann::json &&object) noexcept;
    /// @brief Sets the an accompanying message with the response.
    /// @param[in] message   An accompanying response message.
    void setMessage(const std::string &message) noexcept;
    /// @result The details of the response message.
    [[nodiscard]] std::optional<std::string> getMessage() const noexcept override final;
    /// @result The data portion of the response message.
    [[nodiscard]] std::optional<nlohmann::json> getData() const noexcept override final;

    ~Response() override;
private:
    class ResponseImpl;
    std::unique_ptr<ResponseImpl> pImpl;
};
}
#endif

