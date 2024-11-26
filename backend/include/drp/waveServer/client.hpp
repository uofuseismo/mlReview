#ifndef DRP_WAVE_SERVER_CLIENT_HPP
#define DRP_WAVE_SERVER_CLIENT_HPP
#include <memory>
#include <string>
#include <vector>
namespace DRP::WaveServer
{
 class Request;
 class Waveform;
}
namespace DRP::WaveServer
{
class IClient
{
public:
    virtual ~IClient();
    [[nodiscard]] virtual std::vector<Waveform> getData(const std::vector<Request> &requests) const;
    [[nodiscard]] virtual Waveform getData(const Request &request) const = 0;
    [[nodiscard]] virtual std::string getType() const noexcept = 0;
};
}
#endif
