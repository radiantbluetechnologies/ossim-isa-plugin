cmake_minimum_required (VERSION 2.8)

# Get the library suffix for lib or lib64.
get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)       
if(LIB64)
   set(LIBSUFFIX 64)
else()
   set(LIBSUFFIX "")
endif()

set(requiredLibs ossim_isa_plugin  )
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include")

add_executable(ossim-mspsms ossim-mspsms.cpp )
set_target_properties(ossim-mspsms PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries( ossim-mspsms ${requiredLibs} )
INSTALL(TARGETS ossim-mspsms RUNTIME DESTINATION ${INSTALL_RUNTIME_DIR} BUNDLE DESTINATION ${INSTALL_RUNTIME_DIR} COMPONENT ossim)

