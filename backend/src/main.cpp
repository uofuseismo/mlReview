#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <boost/program_options.hpp>
//#include <soci/soci.h>
#include <spdlog/spdlog.h>
#include <uAuthenticator/uAuthenticator.hpp>
#include "mlReview/database/connection/postgresql.hpp"
#include "mlReview/database/connection/mongodb.hpp"
#include "mlReview/service/handler.hpp"
#include "mlReview/service/catalog/resource.hpp"
#include "mlReview/service/stations/resource.hpp"
#include "mlReview/service/waveforms/resource.hpp"
#include "mlReview/webServer/listener.hpp"

namespace
{

/*
void getWaveform(MLReview::Database::Connection::PostgreSQL &connection)
{
    auto session = reinterpret_cast<soci::session *> (connection.getSession());
    std::string data;
    *session << "SELECT waveform FROM real_time_schema.waveform WHERE identifier = 3821", soci::into(data);  
    std::cout << data << std::endl;
}
*/

}

int main(int argc, char *argv[])
{
    // Make an authenticator
/*
export LDAP_HOST=ldaps://alpha.seis.utah.edu
export LDAP_PORT=636
export LDAP_ORGANIZATION_UNIT="ou=Users"
export LDAP_DOMAIN_COMPONENT="dc=seis,dc=utah,dc=edu"
const std::string &serverAddress,
         int port,
         const std::string &organizationalUnitName,
         const std::string &domainComponent,
         const Version version,
         const TLSVerifyClient tlsVerifyClient,
         const std::string &issuer = "ldap"
*/
std::cout << std::getenv("LDAP_HOST") << std::endl;
    std::shared_ptr<UAuthenticator::IAuthenticator> authenticator
        = std::make_shared<UAuthenticator::LDAP> (
             std::getenv("LDAP_HOST"),
             std::stoi(std::getenv("LDAP_PORT")),
             std::getenv("LDAP_ORGANIZATION_UNIT"),
             std::getenv("LDAP_DOMAIN_COMPONENT"),
             UAuthenticator::LDAP::Version::Three,
             UAuthenticator::LDAP::TLSVerifyClient::Allow,
             "mlReview"); 
/*
    auto result = authenticator->authenticate("user", "password", UAuthenticator::Permissions::ReadWrite);           
    if (result != UAuthenticator::IAuthenticator::ReturnCode::Allowed)
    {
        spdlog::critical("Failed to validate user");
    }
    return 0;
*/

    auto aqmsDatabaseConnection = std::make_shared<MLReview::Database::Connection::PostgreSQL> ();
    aqmsDatabaseConnection->setUser(std::getenv("MLREVIEW_AQMS_DATABASE_READ_ONLY_USER"));
    aqmsDatabaseConnection->setPassword(std::getenv("MLREVEIW_AQMS_DATABASE_READ_ONLY_PASSWORD"));
    aqmsDatabaseConnection->setDatabaseName(std::getenv("MLREVIEW_AQMS_DATABASE_NAME"));
    aqmsDatabaseConnection->setAddress(std::getenv("MLREVIEW_AQMS_DATABASE_HOST"));
    aqmsDatabaseConnection->setPort(std::stoi(std::getenv("MLREVIEW_AQMS_DATABASE_PORT")));
    aqmsDatabaseConnection->setApplication("mlReviewClientBackend");

    auto mongoDatabaseConnection = std::make_shared<MLReview::Database::Connection::MongoDB> ();
    mongoDatabaseConnection->setUser(std::getenv("MLREVIEW_MONGODB_DATABASE_READ_WRITE_USER"));
    mongoDatabaseConnection->setPassword(std::getenv("MLREVIEW_MONGODB_DATABASE_READ_WRITE_PASSWORD"));
    mongoDatabaseConnection->setDatabaseName(std::getenv("MLREVIEW_MONGODB_DATABASE_NAME"));
    mongoDatabaseConnection->setAddress(std::getenv("MLREVIEW_MONGODB_DATABASE_HOST"));
    mongoDatabaseConnection->setPort(std::stoi(std::getenv("MLREVIEW_MONGODB_DATABASE_PORT")));
    mongoDatabaseConnection->setApplication("mlReviewClientBackend");
    mongoDatabaseConnection->connect();

    //getWaveform(*mlDatabaseConnection);

    auto catalogResource
        = std::make_unique<MLReview::Service::Catalog::Resource>
          (mongoDatabaseConnection);
    auto stationsResource
        = std::make_unique<MLReview::Service::Stations::Resource>
          (aqmsDatabaseConnection);
    auto waveformsResource
        = std::make_unique<MLReview::Service::Waveforms::Resource>
          (mongoDatabaseConnection);

    auto handler = std::make_shared<MLReview::Service::Handler> ();
    handler->insert(std::move(catalogResource));
    handler->insert(std::move(stationsResource));
    handler->insert(std::move(waveformsResource));

    const auto address = boost::asio::ip::make_address("127.0.0.1");
    const auto port = static_cast<unsigned short> (8090);
    const auto documentRoot = std::make_shared<std::string> ("./");
    const int nThreads{1};

    // The IO context is required for all I/O
    boost::asio::io_context ioContext{nThreads};
    // The SSL context is required, and holds certificates
    boost::asio::ssl::context context{boost::asio::ssl::context::tlsv12};

    // Create and launch a listening port
    spdlog::info("Launching HTTP listeners...");
    std::make_shared<MLReview::WebServer::Listener> (
        ioContext,
        context,
        boost::asio::ip::tcp::endpoint{address, port},
        documentRoot,
        handler,
        authenticator)->run();

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> instances;
    instances.reserve(nThreads - 1);
    for (int i = nThreads - 1; i > 0; --i)
    {
        instances.emplace_back([&ioContext]
                               {
                                   ioContext.run();
                               });
    }
    ioContext.run();
    return EXIT_SUCCESS;
}
