cmake_minimum_required (VERSION 2.8)

# Get the library suffix for lib or lib64.
get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)       
if(LIB64)
   set(LIBSUFFIX 64)
else()
   set(LIBSUFFIX "")
endif()

SET(LIB_NAME ossim_isa_plugin)

FILE(GLOB OSSIM_ISA_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}  
     "${CMAKE_CURRENT_SOURCE_DIR}/../include/isa/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/../include/isa/atp/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/../include/isa/atp/correlation/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/../include/isa/atp/descriptor/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/../include/isa/services/*.h"    
     "${CMAKE_CURRENT_SOURCE_DIR}/../include/isa/common/*.h"    
     "${CMAKE_CURRENT_SOURCE_DIR}/../include/isa/plugin/*.h")     
     
       
FILE(GLOB OSSIM_ISA_SRCS  RELATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR} 
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/atp/correlation/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/atp/descriptor/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/atp/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/services/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/plugin/*.cpp")     

if(MSP_FOUND)
    FILE(GLOB OSSIM_XMSP_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}  
        "${CMAKE_CURRENT_SOURCE_DIR}/../include/isa/services/msp/*.h")
    set( OSSIM_ISA_HEADERS ${OSSIM_ISA_HEADERS} ${OSSIM_XMSP_HEADERS} )
    FILE(GLOB OSSIM_XMSP_SRCS  RELATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR} 
        "${CMAKE_CURRENT_SOURCE_DIR}/services/msp/*.cpp")
    set( OSSIM_ISA_SRCS ${OSSIM_ISA_SRCS} ${OSSIM_XMSP_SRCS} )
endif(MSP_FOUND)

IF(APPLE)
  FOREACH(f  ${OSSIM_ISA_HEADERS} )
      SET_SOURCE_FILES_PROPERTIES( ${f} PROPERTIES MACOSX_PACKAGE_LOCATION Headers )
  ENDFOREACH(f)
ENDIF(APPLE)

OSSIM_LINK_LIBRARY(${LIB_NAME} COMPONENT_NAME ossim TYPE "${OSSIM_USER_DEFINED_DYNAMIC_OR_STATIC}" 
                   SOURCE_FILES ${OSSIM_ISA_SRCS} 
                   LIBRARIES ${requiredLibs} 
                   HEADERS "${OSSIM_ISA_HEADERS}" 
                   PUBLIC_HEADERS "${OSSIM_ISA_HEADERS}"
                   INSTALL_LIB VERSION_SYMLINKS ADDITIONAL_COMPILE_FLAGS "-DOSSIM_ISA_LIBRARY")

IF(NOT OSSIM_BUILD_FRAMEWORKS OR NOT APPLE)
   install(FILES ${OSSIM_ISA_HEADERS} DESTINATION include/isa COMPONENT ossim) 
ENDIF(NOT OSSIM_BUILD_FRAMEWORKS OR NOT APPLE)
OSSIM_ADD_COMMON_MAKE_UNINSTALL()

