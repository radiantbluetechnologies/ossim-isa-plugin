cmake_minimum_required (VERSION 2.8)

# Get the library suffix for lib or lib64.
get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)       
if(LIB64)
   set(LIBSUFFIX 64)
else()
   set(LIBSUFFIX "")
endif()

set(requiredLibs ${requiredLibs} ossim_isa_plugin  )
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include/isa")

add_executable(isa-plugin-test isa-plugin-test.cpp )
set_target_properties(isa-plugin-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries( isa-plugin-test ${requiredLibs} )

if (MSP_FOUND)
  add_executable(msp-test msp-test.cpp )
  set_target_properties(msp-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
  target_link_libraries( msp-test ${requiredLibs} )
  add_executable(isa-ss-test isa-ss-test.cpp )
  set_target_properties(isa-ss-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
  target_link_libraries( isa-ss-test ${requiredLibs} )
endif(MSP_FOUND)

add_executable(photoblock-json-test photoblock-json-test.cpp )
set_target_properties(photoblock-json-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries( photoblock-json-test ${requiredLibs} )

add_executable(atp-config-test atp-config-test.cpp )
set_target_properties(atp-config-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries( atp-config-test ${requiredLibs} )

