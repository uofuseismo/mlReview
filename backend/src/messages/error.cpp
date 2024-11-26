#include <nlohmann/json.hpp>
#include "drp/messages/error.hpp"
//#include "compress.hpp"

using namespace DRP::Messages;

class Error::ErrorImpl
{
public:
    nlohmann::json pack() const
    {
        nlohmann::json object;
        object["MessageType"] = "DRP::Messages::Error"; 
        if (!mDetails.empty())
        {
            object["Details"] = mDetails;
        }
        if (mHaveCode)
        {
            object["ErrorCode"] = mCode;
        }
        return object;
    }
    std::string mDetails;
    int mCode;
    bool mHaveCode{false};
};

/// Construtor
Error::Error() :
    pImpl(std::make_unique<ErrorImpl> ())
{
}

/// Destructor
Error::~Error() = default;

/// Pack
std::string Error::toString() const noexcept
{
    return pImpl->pack().dump(-1);
}
