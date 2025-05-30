#include <spdlog/spdlog.h>
#include "mlReview/webServer/listener.hpp"
#include "mlReview/service/handler.hpp"
#include "server.hpp"

using namespace MLReview::WebServer;

// Accepts incoming connections and launches the sessions
Listener::Listener(
        boost::asio::io_context& ioContext,
        boost::asio::ssl::context &sslContext,
        boost::asio::ip::tcp::endpoint endpoint, 
        const std::shared_ptr<const std::string> &documentRoot,
        std::shared_ptr<MLReview::Service::Handler> &handler,
        std::shared_ptr<UAuthenticator::IAuthenticator> &authenticator) :
          mIOContext(ioContext),
          mSSLContext(sslContext),
          mAcceptor(boost::asio::make_strand(ioContext)),
          mDocumentRoot(documentRoot),
          mCallbackHandler(handler),
          mAuthenticator(authenticator)
{
    if (mCallbackHandler == nullptr)
    {
        throw std::invalid_argument("Callback handler is NULL");
    }

    boost::beast::error_code errorCode;

    // Open the acceptor
    mAcceptor.open(endpoint.protocol(), errorCode);
    if (errorCode)
    {
        if (errorCode != boost::asio::ssl::error::stream_truncated)
        {
            spdlog::critical("Listener failed to open; failed with: "
                           + std::string {errorCode.what()});
        }
        return;
    }

    // Allow address reuse
    mAcceptor.set_option(boost::asio::socket_base::reuse_address(true),
                         errorCode);
    if (errorCode)
    {
        if (errorCode != boost::asio::ssl::error::stream_truncated)
        {
            spdlog::critical(
                "Listener failed to set reuse_address option; failed with: "
               + std::string {errorCode.what()});
        }
        return;
    }

    // Bind to the server address
    mAcceptor.bind(endpoint, errorCode);
    if (errorCode)
    {
        if (errorCode != boost::asio::ssl::error::stream_truncated)
        {
            spdlog::critical("Listener failed to bind; failed with: "
                           + std::string {errorCode.what()});
        }
        return;
    }

    // Start listening for connections
    mAcceptor.listen(
        boost::asio::socket_base::max_listen_connections, errorCode);
    if (errorCode)
    {
        if (errorCode != boost::asio::ssl::error::stream_truncated)
        {
            spdlog::critical("Listener failed to listen; failed with: "
                           + std::string {errorCode.what()});
        }
        return;
    }
}

void Listener::run()
{
    doAccept(); 
}

void Listener::doAccept()
{
    // The new connection gets its own strand
    mAcceptor.async_accept(
        boost::asio::make_strand(mIOContext),
        boost::beast::bind_front_handler(
           &MLReview::WebServer::Listener::onAccept,
            shared_from_this()));
}

void Listener::onAccept(boost::beast::error_code errorCode,
                        boost::asio::ip::tcp::socket socket)
{
    if (errorCode)
    {
        if (errorCode != boost::asio::ssl::error::stream_truncated)
        {
            spdlog::critical("Listener::onAccept: accept failed with: "
                           + std::string {errorCode.what()});
        }
    }
    else
    {
        // Create the detector session and run it
        std::make_shared<::DetectSession>(
            std::move(socket),
            mSSLContext,
            mDocumentRoot,
            mCallbackHandler,
            mAuthenticator)->run();
    }

    // Accept another connection
    doAccept();
}


