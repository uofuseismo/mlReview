#ifndef DRP_SERVICE_CATALOG_MAGNITUDE_HPP
#define DRP_SERVICE_CATALOG_MAGNITUDE_HPP
#include <string>
#include <memory>
namespace DRP::Service::Catalog
{
class IMagnitude
{
public:
    ~IMagnitude() = default;
    [[nodiscard]] virtual double getSize() const = 0;
    [[nodiscard]] virtual bool haveSize() const noexcept = 0;
    [[nodiscard]] virtual std::string getType() const = 0;
    [[nodiscard]] virtual bool haveType() const noexcept = 0;

    [[nodiscard]] virtual std::unique_ptr<IMagnitude> clone() const noexcept = 0;
};
}
#endif
