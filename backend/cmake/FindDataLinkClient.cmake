# Already in cache, be silent
if (DataLinkClient_INCLUDE_DIR AND DataLinkClient_LIBRARY)
    set(DataLinkClient_FIND_QUIETLY TRUE)
endif()

# Find the include directory
find_path(DataLinkClient_INCLUDE_DIR
          NAMES libdali.h
          HINTS $ENV{DataLinkClient_ROOT}/include
                $ENV{DataLinkClient_ROOT}/
                /usr/local/include)
# Find the library components
find_library(DataLinkClient_LIBRARY
             NAMES dali
             PATHS $ENV{DataLink}/lib/
                   $ENV{DataLink}/
                   /usr/local/lib64
                   /usr/local/lib
            )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DataLinkClient
                                  DEFAULT_MSG DataLinkClient_INCLUDE_DIR DataLinkClient_LIBRARY)
mark_as_advanced(DataLinkClient_INCLUDE_DIR DataLinkClient_LIBRARY)
