#include <iostream>
#include <vector>
#include <memory>
#include <thread>
//#include <soci/soci.h>
#include <spdlog/spdlog.h>
#include <uAuthenticator/uAuthenticator.hpp>
#include "drp/database/connection/postgresql.hpp"
#include "drp/database/connection/mongodb.hpp"
#include "drp/service/handler.hpp"
#include "drp/service/catalog/resource.hpp"
#include "drp/service/stations/resource.hpp"
#include "drp/service/waveforms/resource.hpp"
#include "drp/webServer/listener.hpp"
//#include "drp/service/callback.hpp"
//#include "drp/waveServer/fdsn.hpp"
//#include "drp/waveServer/request.hpp"

namespace
{

/*
void getWaveform(DRP::Database::Connection::PostgreSQL &connection)
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
/*
try
{
DRP::WaveServer::Request request;
request.setNetwork("WY");
request.setStation("YHH");
request.setChannel("HHZ");
request.setLocationCode("01");
request.setStartAndEndTime(std::pair {1729212875, 1729212900});
DRP::WaveServer::FDSN fdsn;//("http://mseedarch.seis.utah.edu:8080/");
fdsn.getData(request);
}
catch (const std::exception &e)
{
spdlog::error(e.what());
}
*/
/*
    auto mlDatabaseConnection = std::make_unique<DRP::Database::Connection::PostgreSQL> ();
    mlDatabaseConnection->setUser(std::getenv("DRP_ML_DATABASE_READ_ONLY_USER"));
    mlDatabaseConnection->setPassword(std::getenv("DRP_ML_DATABASE_READ_ONLY_PASSWORD"));
    mlDatabaseConnection->setDatabaseName(std::getenv("DRP_ML_DATABASE_NAME"));
    mlDatabaseConnection->setAddress(std::getenv("DRP_ML_DATABASE_HOST"));
    mlDatabaseConnection->setPort(std::stoi(std::getenv("DRP_ML_DATABASE_PORT")));
    mlDatabaseConnection->setApplication("drpClientBackend");
    mlDatabaseConnection->connect();
*/

    auto aqmsDatabaseConnection = std::make_shared<DRP::Database::Connection::PostgreSQL> ();
    aqmsDatabaseConnection->setUser(std::getenv("DRP_AQMS_DATABASE_READ_ONLY_USER"));
    aqmsDatabaseConnection->setPassword(std::getenv("DRP_AQMS_DATABASE_READ_ONLY_PASSWORD"));
    aqmsDatabaseConnection->setDatabaseName(std::getenv("DRP_AQMS_DATABASE_NAME"));
    aqmsDatabaseConnection->setAddress(std::getenv("DRP_AQMS_DATABASE_HOST"));
    aqmsDatabaseConnection->setPort(std::stoi(std::getenv("DRP_AQMS_DATABASE_PORT")));
    aqmsDatabaseConnection->setApplication("drpClientBackend");

    auto mongoDatabaseConnection = std::make_shared<DRP::Database::Connection::MongoDB> ();
    mongoDatabaseConnection->setUser(std::getenv("DRP_MONGODB_DATABASE_READ_WRITE_USER"));
    mongoDatabaseConnection->setPassword(std::getenv("DRP_MONGODB_DATABASE_READ_WRITE_PASSWORD"));
    mongoDatabaseConnection->setDatabaseName(std::getenv("DRP_MONGODB_DATABASE_NAME"));
    mongoDatabaseConnection->setAddress(std::getenv("DRP_MONGODB_DATABASE_HOST"));
    mongoDatabaseConnection->setPort(std::stoi(std::getenv("DRP_MONGODB_DATABASE_PORT")));
    mongoDatabaseConnection->setApplication("drpClientBackend");
    mongoDatabaseConnection->connect();

    //getWaveform(*mlDatabaseConnection);

    auto catalogResource
        = std::make_unique<DRP::Service::Catalog::Resource>
          (mongoDatabaseConnection);
    auto stationsResource
        = std::make_unique<DRP::Service::Stations::Resource>
          (aqmsDatabaseConnection);
    auto waveformsResource
        = std::make_unique<DRP::Service::Waveforms::Resource>
          (mongoDatabaseConnection);

    auto handler = std::make_shared<DRP::Service::Handler> ();
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
    std::make_shared<DRP::WebServer::Listener> (
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
