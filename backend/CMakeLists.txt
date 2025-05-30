cmake_minimum_required(VERSION 3.20)
project(MLReviewBackend VERSION 0.1.0 LANGUAGES CXX)
enable_testing()

option(ENABLE_SSL "Enable SSL connections" OFF)
option(WITH_CORS "Compile with CORS *" OFF)
option(UUSS_SRC "Enable UUSS-specific source" ON)
option(COMPILE_DATABASE_UTILITIES "Utilites for interacting with the datbase" OFF)
#cmake -DBUILD_SHARED_LIBS=YES /path/to/source
set(BUILD_SHARED_LIBS YES)

include(CheckCXXCompilerFlag)
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(THREADS_PREFER_PTHREAD_FLAG ON) 
find_package(SOCI COMPONENTS Core PostgreSQL REQUIRED)
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(GeographicLib REQUIRED)
find_package(Boost COMPONENTS program_options date_time iostreams system REQUIRED)
#find_package(jwt-cpp 0.7.0...2.1.0 REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(OpenLdap REQUIRED)
find_package(CURL REQUIRED)
find_package(MiniSEED REQUIRED)
find_package(uAuthenticator REQUIRED)
find_package(DataLinkClient)
find_package(Catch2 3)
find_package(SFF)
find_package(ZLIB)
if (${ENABLE_SSL})
   find_package(OpenSSL COMPONENTS SSL Crypto REQUIRED)
else()
   find_package(OpenSSL COMPONENTS SSL Crypto)
endif()
if (${OPENSSL_FOUND})
   message("Found SSL version ${OPENSSL_VERSION}")
endif()

set(LIBRARY_SRC
    src/service/catalog/arrival.cpp
    src/service/catalog/event.cpp
    src/service/catalog/origin.cpp
    src/service/stations/station.cpp
    src/database/aqms/arrival.cpp
    src/database/aqms/assocaro.cpp
    src/database/aqms/credit.cpp
    src/database/aqms/event.cpp
    src/database/aqms/origin.cpp
    src/database/connection/mongodb.cpp
    src/database/connection/postgresql.cpp
    src/database/machineLearning/arrival.cpp
    src/database/machineLearning/event.cpp
    src/database/machineLearning/origin.cpp
    src/waveServer/client.cpp
    src/waveServer/fdsn.cpp
    src/waveServer/multiClient.cpp
    src/waveServer/request.cpp
    src/waveServer/segment.cpp
    src/waveServer/waveform.cpp)
if (${DataLinkClient_FOUND})
   message("Found DataLinkClient library")
   set(LIBRARY_SRC ${LIBRARY_SRC} src/waveServer/dataLink.cpp)
endif()
add_library(mlReview ${LIBRARY_SRC})
target_link_libraries(mlReview
                      PRIVATE SOCI::Core SOCI::PostgreSQL
                              mongo::mongocxx_shared mongo::bsoncxx_shared
                              GeographicLib::GeographicLib
                              nlohmann_json::nlohmann_json ${LIBXML2_LIBRARIES}
                              ${CURL_LIBRARIES} ${LDAP_LIBRARY} ${LBER_LIBRARY} ${MINISEED_LIBRARY} ${DataLinkClient_LIBRARY})
target_include_directories(mlReview
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
                                   ${CMAKE_CURRENT_SOURCE_DIR}/include
                                   ${PostgreSQL_INCLUDE_DIRS}
                                   ${CURL_INCLUDE_DIR}
                                   ${MINISEED_INCLUDE_DIR}
                                   ${DataLinkClient_INCLUDE_DIR}
                                   ${LIBXML2_INCLUDE_DIR}
                                   Boost::headers)
set_target_properties(mlReview PROPERTIES
                      CXX_STANDARD 20
                      CXX_STANDARD_REQUIRED YES
                      CXX_EXTENSIONS NO)
if (${SFF_FOUND})
   message("Found SFF")
   target_compile_definitions(mlReview PRIVATE WITH_SFF)
   target_include_directories(mlReview PRIVATE ${SFF_INCLUDE_DIRECTORY})
   target_link_libraries(mlReview PRIVATE ${SFF_LIBRARY})
endif()


add_executable(mlReviewBackend
               src/main.cpp
#               src/database/postgresql.cpp
               src/service/handler.cpp
               src/service/resource.cpp
               src/service/actions/acceptEventToAWS.cpp
               src/service/actions/deleteEventFromAWS.cpp
               src/service/catalog/resource.cpp
               src/service/catalog/response.cpp
               src/service/stations/resource.cpp
               src/service/stations/response.cpp
               src/service/waveforms/resource.cpp
               src/service/waveforms/response.cpp
               src/messages/message.cpp
               src/messages/error.cpp
               #src/waveServer/client.cpp
               #src/waveServer/fdsn.cpp
               #src/waveServer/request.cpp
               #src/waveServer/segment.cpp
               #src/waveServer/waveform.cpp
               src/webServer/listener.cpp)
#               src/ldap.cpp
#               src/listener.cpp
#               src/callback.cpp
#               src/authenticator.cpp
#               src/permissions.cpp)

target_link_libraries(mlReviewBackend
                      PUBLIC nlohmann_json::nlohmann_json
                      PRIVATE mlReview
                              SOCI::PostgreSQL
                              Boost::date_time
                              Boost::program_options
                              Boost::iostreams
                              Boost::system
                              spdlog::spdlog
                              GeographicLib::GeographicLib
                              mongo::mongocxx_shared mongo::bsoncxx_shared
                              #jwt-cpp::jwt-cpp
                              ${uAuthenticator_LIBRARY}
                              ${CURL_LIBRARIES} ${LDAP_LIBRARY} ${LBER_LIBRARY} ${MINISEED_LIBRARY} ${SFF_LIBRARY})
target_include_directories(mlReviewBackend
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
                                   ${CMAKE_CURRENT_SOURCE_DIR}/include
                                   ${PostgreSQL_INCLUDE_DIRS}
                                   ${OPENLDAP_INCLUDE_DIR}
                                   ${CURL_INCLUDE_DIR}
                                   ${MINISEED_INCLUDE_DIR}
                                   ${uAuthenticator_INCLUDE_DIR}
                                   Boost::headers)
set_target_properties(mlReviewBackend PROPERTIES
                      CXX_STANDARD 20
                      CXX_STANDARD_REQUIRED YES 
                      CXX_EXTENSIONS NO) 
target_compile_definitions(mlReviewBackend PUBLIC ENABLE_CORS)
if (${OPENSSL_FOUND})
   message("Linking to SSL")
   target_compile_definitions(mlReviewBackend PUBLIC WITH_OPENSSL)
   target_link_libraries(mlReviewBackend PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()
if (${ZLIB_FOUND})
   message("Linking to libz")
   target_compile_definitions(mlReviewBackend PUBLIC WITH_ZLIB)
   target_link_libraries(mlReviewBackend PRIVATE ${ZLIB_LIBRARIES})
   #target_include_directories(mlReviewBackend PRIVATE ${ZLIB_INCLUDE_DIRS})
endif()

if (${COMPILE_DATABASE_UTILITIES})
  add_executable(eventBuilder
                 src/eventBuilder.cpp)
  target_link_libraries(eventBuilder
                        PRIVATE mlReview 
                                SOCI::PostgreSQL SOCI::Core
                                Boost::date_time
                                Boost::program_options
                                spdlog::spdlog
                                nlohmann_json::nlohmann_json
                                mongo::mongocxx_shared mongo::bsoncxx_shared)
  target_include_directories(eventBuilder
                             PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
                                     ${CMAKE_CURRENT_SOURCE_DIR}/include
                                     ${PostgreSQL_INCLUDE_DIRS}
                                     ${OPENLDAP_INCLUDE_DIR}
                                     ${CURL_INCLUDE_DIR}
                                     ${MINISEED_INCLUDE_DIR}
                                     Boost::headers)
  set_target_properties(eventBuilder PROPERTIES
                        CXX_STANDARD 20
                        CXX_STANDARD_REQUIRED YES 
                        CXX_EXTENSIONS NO) 

  add_executable(createEvent
                 src/createEvent.cpp)
  target_link_libraries(createEvent
                        PRIVATE mlReview
                                SOCI::PostgreSQL SOCI::Core
                                Boost::date_time
                                Boost::program_options
                                spdlog::spdlog
                                nlohmann_json::nlohmann_json
                                mongo::mongocxx_shared mongo::bsoncxx_shared)
  target_include_directories(createEvent
                             PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
                                     ${CMAKE_CURRENT_SOURCE_DIR}/include
                                     ${PostgreSQL_INCLUDE_DIRS}
                                     Boost::headers)
  set_target_properties(createEvent PROPERTIES
                        CXX_STANDARD 20
                        CXX_STANDARD_REQUIRED YES 
                        CXX_EXTENSIONS NO) 
endif()


##########################################################################################
#                                      Installation                                      #
##########################################################################################
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
#configure_package_config_file(
#    cmake/${PROJECT_NAME}.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
#    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
#write_basic_package_version_file(
#    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
#    VERSION "${version}"
#    COMPATIBILITY AnyNewerVersion
#)
install(TARGETS mlReviewBackend
        EXPORT ${PROJECT_NAME}-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT Runtime)
#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/mlReviewBackend
#        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
export(EXPORT ${PROJECT_NAME}-targets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}Targets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
#install(FILES
#        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
#        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
#        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})



##########################################################################################
#                                     CPACK Packaging                                    #
##########################################################################################
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "UUSS")
set(CPACK_PACKAGE_CONTACT "ben.baker@utah.edu")
set(CPACK_PACKAGE_LICENSE "MIT")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The backend API for the duty review page.")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/../LICENSE")
set(CPACK_COMPONENTS_ALL executables)
if (WIN32)
    set(CPACK_GENERATOR ZIP WIX)
elseif (APPLE)
    set(CPACK_GENERATOR TGZ productbuild)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CPACK_GENERATOR TGZ RPM)
else()
    set(CPACK_GENERATOR TGZ)
endif()
set(CPACK_SOURCE_IGNORE_FILES
  /\\.git/
  \\.swp
  \\.orig
  /CMakeLists\\.txt\\.user
  /private/
)
include(CPack) # Put this last!

