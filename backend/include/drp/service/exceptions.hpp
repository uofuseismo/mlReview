#ifndef DRP_SERVICE_EXCEPTIONS_HPP
#define DRP_SERVICE_EXCEPTIONS_HPP
#include <exception>
namespace DRP::Service
{
/// @brief This should result in a 403 FORBIDDEN error.
/// @copyright Ben Baker (UUSS) distributed under the MIT license.
class InvalidPermissionException : public std::exception 
{
public:
    InvalidPermissionException(const std::string &message) :
        mMessage(message)
    {   
    }   
    InvalidPermissionException(const char *message) :
        mMessage(message)
    {   
    }   
    ~InvalidPermissionException() final = default;
    virtual const char *what () const noexcept final
    {   
        return mMessage.c_str();
    }   
private:
    std::string mMessage;
};
}
#endif
