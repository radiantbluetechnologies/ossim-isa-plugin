PROJECT(ossim-isa)
Message("################## Setting up ossim-isa  #########################")

cmake_minimum_required(VERSION 2.8)

message(" OSSIM_INSTALL_PREFIX = <${OSSIM_INSTALL_PREFIX}>")
message(" OSSIM_DEV_HOME = <${OSSIM_DEV_HOME}>")

if( NOT DEFINED CMAKE_MODULE_PATH )
   if( EXISTS ${OSSIM_INSTALL_PREFIX} )
      SET(CMAKE_MODULE_PATH "${OSSIM_INSTALL_PREFIX}/share/ossim/CMakeModules")
   elseif( EXISTS ${OSSIM_DEV_HOME} )
      SET(CMAKE_MODULE_PATH "${OSSIM_DEV_HOME}/ossim/cmake/CMakeModules")
   else()
      message( FATAL_ERROR "Need either CMAKE_MODULE_PATH, OSSIM_INSTALL_PREFIX or OSSIM_DEV_HOME defined!" )
   endif()
endif()
message("Using CMAKE_MODULE_PATH = <${CMAKE_MODULE_PATH}> to locate OSSIM components")

include(OssimVersion)
include(OssimCommonVariables)
include(OssimUtilities)

# Ossim - Required:
find_package( ossim )
if( OSSIM_FOUND )
   include_directories( ${OSSIM_INCLUDES} )
   set( requiredLibs ${requiredLibs} ${OSSIM_LIBRARIES} )
   message("Found OSSIM" )
else( OSSIM_FOUND )
   message( FATAL_ERROR "Could not find required Ossim package!" )
endif( OSSIM_FOUND )

find_package( JsonCpp )
if( JSONCPP_FOUND )
   include_directories( ${JSONCPP_INCLUDE_DIR} )
   set( requiredLibs ${requiredLibs} ${JSONCPP_LIBRARY} )
   message("Found JsonCpp" )
else( JSONCPP_FOUND )
   message( FATAL_ERROR "Could not find required JsonCpp package!" )
endif( JSONCPP_FOUND )

message(" MSP_HOME = <${MSP_HOME}>")
find_package(MSP)
if (MSP_FOUND)
   include_directories(${MSP_INCLUDE_DIRS})
   set( requiredLibs ${requiredLibs} ${MSP_LIBRARIES} )
   message( "Found MSP" )
else(MSP_FOUND)
   message(FATAL_ERROR "Could not find MSP!")
endif(MSP_FOUND)

message(" OPENCV_HOME = <${OPENCV_HOME}>")
find_package(OpenCV)
if( OPENCV_FOUND )
   include_directories( ${OPENCV_INCLUDE_DIR} )
   set( requiredLibs ${requiredLibs} ${OPENCV_LIBRARIES} )
   message( "Found OpenCV" )
else( OPENCV_FOUND )
     message( FATAL "Could not find OpenCV package. " )
endif( OPENCV_FOUND )

IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
  FIND_PACKAGE(Git)
  IF(GIT_FOUND)
    EXECUTE_PROCESS(
      COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      OUTPUT_VARIABLE "OSSIM_ISA_GIT_REVISION"
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    MESSAGE( STATUS "Git revision: ${OSSIM_ISA_GIT_REVISION}" )
  ELSE(GIT_FOUND)
    SET(ossimisa_BUILD_VERSION 0)
  ENDIF(GIT_FOUND)
ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
TODAYS_DATE(OSSIM_ISA_BUILD_DATE)
set(OSSIM_ISA_BUILD_DATE "\"${OSSIM_ISA_BUILD_DATE}\"")

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/include/isa/Config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/isa/Config.h @ONLY)

set (CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src )
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/apps )
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test )

# Share install targets (configuration JSON files):
FILE(GLOB ossim_isa_json ${${PROJECT_NAME}_SOURCE_DIR}/share/ossim-isa/*.json)
INSTALL(FILES ${ossim_isa_json} DESTINATION share/ossim/isa COMPONENT ossim)


message("################## end of ossim-isa setup #########################")
