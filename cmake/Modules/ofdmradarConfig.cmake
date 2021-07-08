if(NOT PKG_CONFIG_FOUND)
    INCLUDE(FindPkgConfig)
endif()
PKG_CHECK_MODULES(PC_OFDMRADAR ofdmradar)

FIND_PATH(
    OFDMRADAR_INCLUDE_DIRS
    NAMES ofdmradar/api.h
    HINTS $ENV{OFDMRADAR_DIR}/include
        ${PC_OFDMRADAR_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    OFDMRADAR_LIBRARIES
    NAMES gnuradio-ofdmradar
    HINTS $ENV{OFDMRADAR_DIR}/lib
        ${PC_OFDMRADAR_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/ofdmradarTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OFDMRADAR DEFAULT_MSG OFDMRADAR_LIBRARIES OFDMRADAR_INCLUDE_DIRS)
MARK_AS_ADVANCED(OFDMRADAR_LIBRARIES OFDMRADAR_INCLUDE_DIRS)
