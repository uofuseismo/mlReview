#ifndef DRP_SERVICE_STATIONS_RESOURCE_HPP
#define DRP_SERVICE_STATIONS_RESOURCE_HPP
#include <memory>
#include <drp/service/resource.hpp>
namespace DRP::Database::Connection
{
 class PostgreSQL;
}
namespace DRP::Service::Stations
{
/// @class Resource "resource.hpp" "drp/service/stations/resource.hpp"
/// @brief The station resource is responsible for returning station 
///        information.
/// @copyright Ben Baker (University of Utah) distributed under the MIT license.
class Resource : public DRP::Service::IResource
{
public:
    explicit Resource(std::shared_ptr<DRP::Database::Connection::PostgreSQL> &postgresClient);

    /// @brief Destructor
    ~Resource() override;
    /// @brief Processes the user request.
    [[nodiscard]] std::unique_ptr<DRP::Messages::IMessage> processRequest(const nlohmann::json &request) override;
    /// @result The resource's name.
    [[nodiscard]] std::string getName() const noexcept override final;
    /// @result The resource's documentation.
    //[[nodiscard]] std::string getDocumentation() const noexcept override final;

    Resource(const Resource &) = delete;
    Resource& operator=(const Resource &) = delete;
private:
    class ResourceImpl;
    std::unique_ptr<ResourceImpl> pImpl;
};
}
#endif
