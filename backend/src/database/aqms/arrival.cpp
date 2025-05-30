#include <iostream>
#include <string>
#include <algorithm>
#include <boost/format.hpp>
#include "mlReview/database/aqms/arrival.hpp"

namespace
{

std::string convertString(const std::string &input)
{
    auto result = input;
    std::remove_if(result.begin(), result.end(), ::isspace);
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

}

using namespace MLReview::Database::AQMS;

class Arrival::ArrivalImpl
{
public:
    double mTime;
    double mQuality{-1}; // Valid range is [0, 1]
    int64_t mIdentifier;
    std::string mAuthority;
    std::string mNetwork;
    std::string mStation;
    std::string mChannel;
    std::string mLocationCode;
    std::string mPhase;
    std::string mSubSource;
    ReviewFlag mReviewFlag;
    FirstMotion mFirstMotion{FirstMotion::Unknown};
    bool mHaveIdentifier{false};
    bool mHaveLocationCode{false};
    bool mHaveReviewFlag{false};
    bool mHaveTime{false};
};

/// Constructor
Arrival::Arrival() :
    pImpl(std::make_unique<ArrivalImpl> ())
{
}

/// Copy constructor
Arrival::Arrival(const Arrival &arrival)
{
    *this = arrival;
}

/// Move construtor
Arrival::Arrival(Arrival &&arrival) noexcept
{
    *this = std::move(arrival);
}

/// Copy assignment
Arrival& Arrival::operator=(const Arrival &arrival)
{
    if (&arrival == this){return *this;}
    pImpl = std::make_unique<ArrivalImpl> (*arrival.pImpl);
    return *this;
}

/// Move assignment
Arrival& Arrival::operator=(Arrival &&arrival) noexcept
{
    if (&arrival == this){return *this;}
    pImpl = std::move(arrival.pImpl);
    return *this;
}

/// Reset class 
void Arrival::clear() noexcept
{
    pImpl = std::make_unique<ArrivalImpl> ();
}

/// Destructor
Arrival::~Arrival() = default;

/// Authority
void Arrival::setAuthority(const std::string &authorityIn)
{
    auto authority = ::convertString(authorityIn);
    if (authority.empty()){throw std::invalid_argument("Authority is empty");}
    if (authority.size() > 15)
    {
        throw std::invalid_argument("Authority must be 15 characters or less");
    }
    pImpl->mAuthority = authority;
}

std::string Arrival::getAuthority() const
{
    if (!haveAuthority()){throw std::runtime_error("Authority not set");}
    return pImpl->mAuthority;
}

bool Arrival::haveAuthority() const noexcept
{
    return !pImpl->mAuthority.empty();
}

/// Station
void Arrival::setStation(const std::string &stationIn)
{
    auto station = ::convertString(stationIn);
    if (station.empty()){throw std::invalid_argument("Station is empty");}
    if (station.size() > 6)
    {
        throw std::invalid_argument("Station must be 6 characters or less");
    }
    pImpl->mStation = station;
}

std::string Arrival::getStation() const
{
    if (!haveStation()){throw std::runtime_error("Station not set");}
    return pImpl->mStation;
}

bool Arrival::haveStation() const noexcept
{
    return !pImpl->mStation.empty();
}

/// Time
void Arrival::setTime(const std::chrono::microseconds &time) noexcept
{
    setTime(time.count()*1.e-6);
}

void Arrival::setTime(const double time) noexcept
{
    pImpl->mTime = time;
    pImpl->mHaveTime = true;
}

double Arrival::getTime() const
{
    if (!haveTime()){throw std::runtime_error("Time not set");}
    return pImpl->mTime;
}

bool Arrival::haveTime() const noexcept
{
    return pImpl->mHaveTime;
}

/// Network
void Arrival::setNetwork(const std::string &networkIn)
{
    auto network = ::convertString(networkIn);
    if (network.empty()){throw std::invalid_argument("Network is empty");}
    if (network.size() > 8)
    {
        throw std::invalid_argument("Network must be 8 characters or less");
    }
    pImpl->mNetwork = network;
}

std::optional<std::string> Arrival::getNetwork() const noexcept
{
    return !pImpl->mNetwork.empty() ?
           std::optional<std::string> {pImpl->mNetwork} : std::nullopt;
}

/// SEED channel
void Arrival::setSEEDChannel(const std::string &channelIn)
{
    auto channel = ::convertString(channelIn);
    if (channel.empty()){throw std::invalid_argument("Channel is empty");}
    if (channel.size() > 3)
    {   
        throw std::invalid_argument(
            "SEED channel must be 3 characters or less");
    }   
    pImpl->mChannel = channel;
}

std::optional<std::string> Arrival::getSEEDChannel() const noexcept 
{
    return !pImpl->mChannel.empty() ?
           std::optional<std::string> {pImpl->mChannel} : std::nullopt;
}

/// Location code
void Arrival::setLocationCode(const std::string &locationCode)
{
    if (locationCode.size() > 2)
    {
        throw std::invalid_argument(
             "Location code must be 2 characters or less");
    }
    pImpl->mLocationCode = locationCode;
    pImpl->mHaveLocationCode = true;
}

std::optional<std::string> Arrival::getLocationCode() const noexcept 
{
    return pImpl->mHaveLocationCode ?
           std::optional<std::string> {pImpl->mLocationCode} : std::nullopt;
}

/// Phase
void Arrival::setPhase(const std::string &phaseIn)
{
    auto phase = phaseIn;
    std::remove_if(phase.begin(), phase.end(), ::isspace);
    if (phase.empty()){throw std::invalid_argument("Phase cannot be empty");}
    if (phase.size() > 8)
    {
        throw std::invalid_argument("Phase must be 8 characters or less");
    }
    pImpl->mPhase = phase;
}

std::optional<std::string> Arrival::getPhase() const noexcept
{
    return !pImpl->mPhase.empty() ?
           std::optional<std::string> {pImpl->mPhase} : std::nullopt;
}

/// Quality
void Arrival::setQuality(double quality)
{
    if (quality < 0 || quality > 1)
    {
        throw std::invalid_argument("Quality must be in range [0,1]");
    }
    pImpl->mQuality = quality;
}

std::optional<double> Arrival::getQuality() const noexcept
{
    return pImpl->mQuality > -1 ?
           std::optional<double> {pImpl->mQuality} : std::nullopt;
}

/// First motion
void Arrival::setFirstMotion(const Arrival::FirstMotion firstMotion) noexcept
{
    pImpl->mFirstMotion = firstMotion;
}

Arrival::FirstMotion Arrival::getFirstMotion() const noexcept
{
    return pImpl->mFirstMotion;
}

/// Identifier
void Arrival::setIdentifier(const int64_t identifier) noexcept
{
    pImpl->mIdentifier = identifier;
    pImpl->mHaveIdentifier = true;
}

std::optional<int64_t> Arrival::getIdentifier() const noexcept
{
    return pImpl->mHaveIdentifier ?
           std::optional<int64_t> {pImpl->mIdentifier} : std::nullopt;
}

/// Review flag
void Arrival::setReviewFlag(const Arrival::ReviewFlag reviewFlag) noexcept
{
    pImpl->mReviewFlag = reviewFlag;
    pImpl->mHaveReviewFlag = true;
}

std::optional<Arrival::ReviewFlag> Arrival::getReviewFlag() const noexcept
{
    return pImpl->mHaveReviewFlag ?
           std::optional<Arrival::ReviewFlag> {pImpl->mReviewFlag} :
           std::nullopt;
}

/// Subsource
void Arrival::setSubSource(const std::string &subSource)
{
    if (subSource.size() > 8)
    {
        throw std::invalid_argument("Sub-source must be 8 characters or less");
    }
    pImpl->mSubSource = subSource;
}

std::optional<std::string> Arrival::getSubSource() const noexcept
{
    return !pImpl->mSubSource.empty() ?
           std::optional<std::string> {pImpl->mSubSource} :
           std::nullopt;       
}

/// Insertion string
std::string MLReview::Database::AQMS::toInsertString(const Arrival &arrival)
{
    if (!arrival.haveStation())
    {
        throw std::invalid_argument("Station is not set");
    } 
    if (!arrival.haveTime())
    {
        throw std::invalid_argument("Arrival time not set");
    }
    if (!arrival.haveAuthority())
    {
        throw std::invalid_argument("Authority not set");
    }
    std::string insertStatement{"INSERT INTO arrival "};
/*
    std::string keys;
    double mTime;
    double mQuality{-1}; // Valid range is [0, 1]
    int64_t mIdentifier;
    std::string mAuthority;
    std::string mNetwork;
    std::string mStation;
    std::string mChannel;
    std::string mLocationCode;
    std::string mPhase;
    std::string mSubSource;
    ReviewFlag mReviewFlag;
    FirstMotion mFirstMotion{FirstMotion::Unknown};
    bool mHaveIdentifier{false};
    bool mHaveLocationCode{false};
    bool mHaveReviewFlag{false};
    bool mHaveTime{false};
    std::string identifier{"NULL"};
    if (arrival.getIdentifier()){identifier = std::to_string(*arrival.getIdentifier());}
    std::string network{"NULL"};
    if (arrival.getNetwork()){network = *arrival.getNetwork();}
    std::string channel{"NULL"};
    if (arrival.getSEEDChannel()){channel = *arrival.getSEEDChannel();}
    std::string location{"NULL"};
    if (arrival.getLocationCode()){location = *arrival.getLocationCode();}
        
    std::string subsource{"NULL"};
    if (arrival.getSubSource()){subsource = *arrival.getSubSource();}
*/

    std::string keys = {"(auth, datetime, sta,"};
    std::string values
        = str(boost::format(" VALUES ('%1%', TrueTime.putEpoch(%2$.6f, 'NOMINAL'), '%3%',")
              %arrival.getAuthority()
              %arrival.getTime()
              %arrival.getStation()
             );
    if (arrival.getIdentifier())
    {
        keys = keys + " arid,";
        values = values + str(boost::format(" %1$d,")% *arrival.getIdentifier() );
    }
    if (arrival.getSubSource())
    {
        keys = keys + " subsource,";
        values = values + str(boost::format(" '%1%',")% *arrival.getSubSource() ); 
    }
    if (arrival.getNetwork())
    {   
        keys = keys + " net,";
        values = values + str(boost::format(" '%1%',")% *arrival.getNetwork() );
    }
    if (arrival.getSEEDChannel())
    {
        keys = keys + " seedchan, channel,";
        auto seedChannel = *arrival.getSEEDChannel();
        values = values + str(boost::format(" '%1%', '%2%',")
                             %seedChannel%seedChannel);
    }
    if (arrival.getLocationCode())
    {
        keys = keys + " location,";
        values = values + str(boost::format(" '%1%',")% *arrival.getLocationCode() );
    }
    if (arrival.getPhase())
    {
        keys = keys + " iphase,";
        values = values + str(boost::format(" '%1%',")% *arrival.getPhase() );
    }
    if (arrival.getFirstMotion() == Arrival::FirstMotion::Up)
    {
        keys = keys + " fm,";
        values = values + " 'c.',";
    }
    else if (arrival.getFirstMotion() == Arrival::FirstMotion::Down)
    {
        keys = keys + " fm,";
        values = values + " 'd.',";
    }
    if (arrival.getQuality())
    {
        keys = keys + " quality,";
        values = values + str(boost::format(" %1$.3f,")% *arrival.getQuality() );
    }
    if (arrival.getReviewFlag())
    {
        if (*arrival.getReviewFlag() == Arrival::ReviewFlag::Automatic)
        {
            keys = keys + " rflag,";
            values = values + " 'A',";
        }
        else if (*arrival.getReviewFlag() == Arrival::ReviewFlag::Human)
        {
            keys = keys + " rflag,";
            values = values + " 'H',";
        }
        else if (*arrival.getReviewFlag() == Arrival::ReviewFlag::Finalized)
        {   
            keys = keys + " rflag,";
            values = values + " 'F',";
        }
    }
    // Tack this on
    keys = keys + " channelsrc,";
    values = values + " 'SEED',";

    // Delete the trailing comma and make it a ")"
    keys.back() = ')';
    values.back()= ')';
//                  %arrival.getAuthority()
                  //%arrival.getTime()
//                  %arrival.getStation();
    // Now build up the rest
    
//                         arrival.getAuthority(),
//                         arrival.getTime(),
//                         arrival.getStation());
    // Put it all together
    insertStatement = insertStatement + keys + values + ";";
    return insertStatement;
}

