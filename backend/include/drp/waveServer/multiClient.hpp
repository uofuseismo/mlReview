#ifndef DRP_WAVE_SERVER_MULTI_CLIENT_HPP
#define DRP_WAVE_SERVER_MULTI_CLIENT_HPP
#include <memory>
#include <drp/waveServer/client.hpp>
namespace DRP::WaveServer
{
 class Waveform;
}
namespace DRP::WaveServer
{
class MultiClient : public IClient
{
public:
    MultiClient();
    void insert(std::unique_ptr<IClient> &&client, int priority);
    [[nodiscard]] std::vector<Waveform> getData(const std::vector<Request> &requests) const override final;
    [[nodiscard]] Waveform getData(const Request &request) const override final;
    [[nodiscard]] std::string getType() const noexcept override final;
    ~MultiClient() override;
private:
    class MultiClientImpl;
    std::unique_ptr<MultiClientImpl> pImpl;
};
}
#endif
