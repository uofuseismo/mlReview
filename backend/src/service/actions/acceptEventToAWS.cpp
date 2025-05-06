#include <string>
#include <vector>
#include <spdlog/spdlog.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include "mlReview/service/actions/acceptEventToAWS.hpp"
#include "mlReview/database/connection/mongodb.hpp"
#include "mlReview/messages/message.hpp"
#include "mlReview/messages/error.hpp"

#define RESOURCE_NAME "actions/acceptToAWS"

using namespace MLReview::Service::Actions;

namespace
{
class Response final : public MLReview::Messages::IMessage
{
public:
    explicit Response(const std::string &message) :
        mMessage(message)
    {
    }
    int getStatusCode() const noexcept final
    {
        return 200;
    }
    bool getSuccess() const noexcept final
    {
        return true;
    }
    std::optional<std::string> getMessage() const noexcept
    {
        return std::optional<std::string> {mMessage};
    }
    ~Response() final = default;
    std::string mMessage;
};
}

/// Generates a catalog from the application database
bool checkIfEventExists(
    MLReview::Database::Connection::MongoDB &connection,
    const int64_t identifier,
    const std::string &collectionName)
{
    auto databaseName = connection.getDatabaseName();
    using namespace bsoncxx::builder::basic;
    auto client
        = reinterpret_cast<mongocxx::client *> (connection.getSession());
    auto database = client->database(databaseName);
    if (database)
    {
        auto collection = database.collection(collectionName);
        if (collection)
        {
            mongocxx::options::find searchOptions{};
            searchOptions.projection(
                make_document(kvp("parametricData", 0), 
                              kvp("_id", 0)) 
            );
            auto filterKey
                = bsoncxx::document::view_or_value(
                     make_document(kvp("eventIdentifier", identifier)));
            auto foundDocument = collection.find_one(filterKey, searchOptions);
            return foundDocument ? true : false;
        }
    }
    else
    {
        throw std::runtime_error("Database connection is null");
    }
    return false;
}

[[nodiscard]]
nlohmann::json getParametricData(
    MLReview::Database::Connection::MongoDB &connection,
    const int64_t mongoIdentifier,
    const std::string &collectionName)
{
    nlohmann::json jsonObject;
    auto databaseName = connection.getDatabaseName();
    using namespace bsoncxx::builder::basic;
    auto client
        = reinterpret_cast<mongocxx::client *> (connection.getSession());
    auto database = client->database(databaseName);
    if (database)
    {
        auto collection = database.collection(collectionName);
        if (collection)
        {   
            mongocxx::options::find searchOptions{};
            searchOptions.projection(
                make_document(kvp("waveformData", 0), 
                              kvp("_id", 0)) 
            );  
            auto filterKey
                = bsoncxx::document::view_or_value(
                     make_document(kvp("eventIdentifier", mongoIdentifier)));
            auto foundDocument = collection.find_one(filterKey, searchOptions);
            if (foundDocument)
            {
                auto json
                   = bsoncxx::to_json(*foundDocument,
                                      bsoncxx::ExtendedJsonMode::k_relaxed);
                jsonObject = nlohmann::json::parse(json);
            }
            else
            {
                throw std::runtime_error("Could not find event "
                                       + std::to_string(mongoIdentifier));
            }
        }
        else
        {
            throw std::runtime_error("Collection " 
                                   + collectionName + " does not exist");
        }
    }
    else
    {
        throw std::runtime_error("Database connection is null");
    }
    return jsonObject;
}

[[nodiscard]]
std::pair<bool, nlohmann::json>
   initialResponseToAPI(const nlohmann::json &initialResponse,
                        const std::string &authorityIn = "UU",
                        const bool isHumanReviewed = true)
{
   auto authority = authorityIn;
   std::transform(authority.begin(), authority.end(), authority.begin(),
                  [](auto c){return std::tolower(c);});

   nlohmann::json result;
   auto eventIdentifier = initialResponse["eventIdentifier"].template get<int64_t> ();
   auto parametricData = initialResponse["parametricData"];
   auto preferredOrigin = parametricData["preferredOrigin"];
   result["identifier"] = "urts" + std::to_string(eventIdentifier);
   auto aqmsIdentifiers = initialResponse["aqmsEventIdentifiers"].template get<std::vector<int64_t>> ();
   int64_t aqmsIdentifier{-1};
   if (!aqmsIdentifiers.empty())
   {
       for (auto &identifier : aqmsIdentifiers)
       {
           if (aqmsIdentifier >= 80000000 && aqmsIdentifier < 90000000)
           {
               aqmsIdentifier = identifier;
               break;
           }
       }
   }
   if (aqmsIdentifier >= 80000000)
   {
       result["comcatIdentifier"] = authority + std::to_string(aqmsIdentifier);
   }
   else
   {
       result["comcatIdentifier"] = nullptr;
   }
   result["latitudeDeg"] = preferredOrigin["latitude"].template get<double> ();
   result["longitudeDeg"] = preferredOrigin["longitude"].template get<double> ();
   result["depthKM"] = preferredOrigin["depth"].template get<double> ()*1.e-3;
   result["originTimeUTC"] = preferredOrigin["time"].template get<double> ();
   result["authority"] = authority;
   result["humanReviewed"] = isHumanReviewed;
   result["automaticOrigin"] = true;
   result["locatorAlgorithm"] = preferredOrigin["algorithm"].template get<std::string> ();
   result["catalog"] = "urts";
   result["magnitude"] = nullptr;
   result["magnitudeType"] = nullptr;
   auto eventType = parametricData["eventType"].template get<std::string> ();
   std::transform(eventType.begin(), eventType.end(), eventType.begin(),
                  [](auto c){return std::tolower(c);});
   if (eventType == "earthquake")
   {
       result["eventType"] = "eq"; 
   }
   else if (eventType == "quarryBlast")
   {
       result["eventType"] = "qb";
   }
   else
   {
       spdlog::warn("Unhandled event type: " + eventType);
       result["eventType"] = "unknown"; 
   }
   // Arrivals
   nlohmann::json arrivals;
   for (const auto &inputArrival : preferredOrigin["arrivals"])
   {
       nlohmann::json arrival;
       if (inputArrival.contains("network") &&
           inputArrival.contains("station") &&
           inputArrival.contains("phase") &&
           inputArrival.contains("time"))
       {
           auto network = inputArrival["network"].template get<std::string> ();
           auto station = inputArrival["station"].template get<std::string> (); 
           auto phase = inputArrival["phase"].template get<std::string> ();
           auto time = inputArrival["time"].template get<double> ();
           std::string channel;
           if (phase == "P")
           {
               channel = inputArrival["channel1"].template get<std::string> ();
           }
           else if (phase == "S")
           {
               if (inputArrival.contains("channel2"))
               {
                   channel = inputArrival["channel2"].template get<std::string> ();
               }
               else
               {
                   channel = inputArrival["channel1"].template get<std::string> ();
               }
           } 
           else
           {
               spdlog::warn("Unhandled phase " + phase);
               continue;
           }
           auto identifier = network  + station + channel;
           std::string locationCode;
           if (inputArrival.contains("locationCode"))
           {
               locationCode = inputArrival["locationCode"].template get<std::string> ();
               if (!locationCode.empty())
               {
                   identifier = identifier + locationCode;
               }
           }
           identifier = identifier + phase + "-" + std::to_string(static_cast<int64_t> (time*1e6));;
           arrival["identifier"] = identifier;
           arrival["network"] = network;
           arrival["station"] = station;
           arrival["channel"] = channel;
           if (!locationCode.empty())
           {
               arrival["locationCode"] = locationCode;
           }
           else
           {
               arrival["locationCode"] = nullptr;
           }
           arrival["phase"] = phase;
           arrival["time"] = time;
           arrival["uncertainty"] = nullptr;
           arrival["algorithm"] = "unet-detection,cnn-refinement";
           arrival["isAutomatic"] = true;
           arrivals.push_back(arrival);
       }
       else
       {
           spdlog::warn("Skipping arrival");
       }
   }
   if (!arrivals.empty()){result["arrivals"] = arrivals;}

   auto monitoringRegion = parametricData["monitoringRegion"].template get<std::string> ();
   std::transform(monitoringRegion.begin(), monitoringRegion.end(), monitoringRegion.begin(),
                  [](auto c){return std::tolower(c);});

   bool isYellowstone{true};
   if (monitoringRegion == "yellowstone")
   {
       isYellowstone = true;
   }
   else if (monitoringRegion == "utah")
   {
       isYellowstone = false;
   }
   else
   {
       throw std::runtime_error("Unhandled monitoring region "
                              + monitoringRegion);
   }
   return std::pair{isYellowstone, result};
}

/// Let MongoDB know the event was submitted to AWS
void updateMongoDB(
    MLReview::Database::Connection::MongoDB &connection,
    const int64_t mongoIdentifier,
    const std::string &collectionName)
{
    auto databaseName = connection.getDatabaseName();
    using namespace bsoncxx::builder::basic;
    auto client
        = reinterpret_cast<mongocxx::client *> (connection.getSession());
    auto database = client->database(databaseName);
    if (database)
    {   
        auto collection = database.collection(collectionName);
        if (collection)
        {
            mongocxx::options::find searchOptions{};
            searchOptions.projection(
                make_document(kvp("waveformDataData", 0),
                              kvp("_id", 0))
            );
            auto filterKey
                = bsoncxx::document::view_or_value(
                     make_document(kvp("eventIdentifier", mongoIdentifier)));
            auto foundDocument = collection.find_one(filterKey, searchOptions);
            if (!foundDocument)
            {
                throw std::runtime_error("Could not find event "
                                       + std::to_string(mongoIdentifier));
            }
            // Update
            mongocxx::options::update updateOptions;
            updateOptions.upsert(false);
            nlohmann::json object;
            object["submittedToCloudCatalog"] = true;
            bsoncxx::document::value bsonObject
                = bsoncxx::from_json(object.dump());
            auto updateDocument
                = make_document(kvp("$set", bsonObject));
            auto updateResult
                = collection.update_one(filterKey.view(),
                                        updateDocument.view(),
                                        updateOptions);
            spdlog::info("Updated "
                       + std::to_string(mongoIdentifier));
        }
        else
        {
            throw std::runtime_error("Collection "
                                   + collectionName + " does not exist");
        }
    }
    else
    {
        throw std::runtime_error("Database connection is null");
    }
}



class AcceptEventToAWS::AcceptEventToAWSImpl
{
public:
    AcceptEventToAWSImpl(
        std::shared_ptr<MLReview::Database::Connection::MongoDB> &mongoConnection) :
    mMongoDBConnection(mongoConnection)
    {
        if (mMongoDBConnection == nullptr)
        {
            throw std::invalid_argument("MongoDB connection is NULL");
        }   
        auto apiURL = std::getenv("MLREVIEW_AWS_API_URL");
        if (apiURL)
        {
            mAPIURL = std::string {apiURL};
        }
        else
        {
            throw std::runtime_error(
               "Could not read MLREVIEW_AWS_API_URL environment variable");
        }
        if (mAPIURL.empty())
        {
            throw std::runtime_error("MLREVIEW_AWS_API_URL is empty");
        }
        if (mAPIURL.back() != '/'){mAPIURL.push_back('/');}
        auto apiKey = std::getenv("MLREVIEW_AWS_API_ACCESS_KEY");
        if (apiKey)
        {
            mAPIAccessKey = std::string {apiKey};
        }
        else
        {
            throw std::runtime_error(
                "Could not read MLREVIEW_AWS_API_ACCESS_KEY environment variable");
        }
        if (mAPIAccessKey.empty())
        {
            throw std::runtime_error("MLREVIEW_AWS_API_ACCESS_KEY is empty");
        }
    }
    void sendRequest(const nlohmann::json &data, bool isYellowstone)
    {
        std::string errorMessage;
        CURL *curl{nullptr};
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        if (curl)
        {
            std::string url;
            if (isYellowstone)
            {
                url = mAPIURL + "Yellowstone";
            }
            else
            {
                url = mAPIURL + "Utah";
            }
            spdlog::debug("Endpoint is " + url);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            struct curl_slist *headerList{nullptr};
            if (!mAPIAccessKey.empty())
            {
                auto xAPIKeyHeader = "x-api-key:" + mAPIAccessKey;
                headerList = curl_slist_append(headerList, xAPIKeyHeader.c_str());
            }
            headerList = curl_slist_append(headerList, "Accept: application/json");
            headerList = curl_slist_append(headerList, "Content-Type: application/json");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
            auto stringResult = data.dump(-1);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, stringResult.c_str());
            //curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ="name=danial&project=curl");
            auto result = curl_easy_perform(curl);
            if (result != CURLE_OK)
            {
                errorMessage = std::string {curl_easy_strerror(result)};
            }
            curl_slist_free_all(headerList);
            curl_easy_cleanup(curl);
        }   
        else
        {
            errorMessage = "Error initializing curl";
        }   
        curl_global_cleanup();
        if (!errorMessage.empty())
        {
            throw std::runtime_error("CURL request failed with: " + errorMessage);
        }
    }

    std::shared_ptr<MLReview::Database::Connection::MongoDB>
        mMongoDBConnection{nullptr};
    std::string mAPIURL;
    std::string mAPIAccessKey;
};

/// Constructor
AcceptEventToAWS::AcceptEventToAWS(
    std::shared_ptr<MLReview::Database::Connection::MongoDB> &mongoConnection) :
    pImpl(std::make_unique<AcceptEventToAWSImpl> (mongoConnection))
{
}

/// Destructor
AcceptEventToAWS::~AcceptEventToAWS() = default;

/// Resource name
std::string AcceptEventToAWS::getName() const noexcept
{
    return RESOURCE_NAME;
}

/// Process request
std::unique_ptr<MLReview::Messages::IMessage> 
AcceptEventToAWS::processRequest(const nlohmann::json &request)
{
    // Figure out the query information
    auto now = std::chrono::duration_cast<std::chrono::seconds> (
        std::chrono::system_clock::now().time_since_epoch()); 
    int64_t mongoIdentifier{-1};
    if (!request.contains("identifier"))
    {   
        throw std::invalid_argument("Event identifier not set");
    }   
    mongoIdentifier = request["identifier"].template get<int64_t> (); 
    // Does the event exist?
    const std::string collectionName{"events"};
    auto eventExists = ::checkIfEventExists(*pImpl->mMongoDBConnection,
                                            mongoIdentifier,
                                            collectionName);
    if (eventExists)
    {
        auto initialJSON = ::getParametricData(*pImpl->mMongoDBConnection,
                                               mongoIdentifier,
                                               collectionName);
        auto [isYellowstone, jsonForAPI] = ::initialResponseToAPI(initialJSON,
                                                 "UU",
                                                 true);
        // Send it
        try
        {
            if (isYellowstone)
            {
                spdlog::info("Propagating to Yellowstone endpoint");
            }
            else
            {
                spdlog::info("Propagating to Utah endpoint");
            }
            pImpl->sendRequest(jsonForAPI, isYellowstone);
        }
        catch (const std::exception &e)
        {
            // Return 500 error
        }
    }
    else
    {
        throw std::invalid_argument("Event identifier "
                                  + std::to_string(mongoIdentifier)
                                  + " does not exist");
    }

    auto responseMessage = "Successfully propagated "
                         + std::to_string(mongoIdentifier)
                         + " to AWS";
    auto response = std::make_unique<::Response> (responseMessage); 
    return response;
}
