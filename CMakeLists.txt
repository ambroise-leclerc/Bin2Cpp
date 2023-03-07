cmake_minimum_required(VERSION 3.25)

project("bin2cpp")
set(BIN2CPP_VERSION 0.1.0)
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_23)
target_compile_options(project_options INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(HAS_STD_FORMAT True)
else()
  set(HAS_STD_FORMAT False)
endif()
message("Has std::format : ${HAS_STD_FORMAT}")


set(CPM_DOWNLOAD_VERSION 0.34.3)
if(CPM_SOURCE_CACHE)
  get_filename_component(CPM_SOURCE_CACHE ${CPM_SOURCE_CACHE} ABSOLUTE)
  set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif(DEFINED ENV{CPM_SOURCE_CACHE})
  set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else()
  set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif()
if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
  message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
  file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()
include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage("gh:jarro2783/cxxopts@3.1.0")


add_executable(bin2cpp bin2cpp.cpp)
target_link_libraries(bin2cpp PRIVATE project_options cxxopts::cxxopts)

if (NOT ${HAS_STD_FORMAT})
  CPMAddPackage("gh:fmtlib/fmt#7.1.3")
  target_link_libraries(bin2cpp PRIVATE fmt)
endif()

include(CTest)
add_test(NAME FromSourceCodeHppFile_8bits COMMAND  $<TARGET_FILE:bin2cpp> ${CMAKE_CURRENT_SOURCE_DIR}/SourceCode.hpp -s -b 8) 
add_test(NAME FromSourceCodeHppFile_16bits COMMAND  $<TARGET_FILE:bin2cpp> ${CMAKE_CURRENT_SOURCE_DIR}/SourceCode.hpp -s -b 16) 
add_test(NAME FromSourceCodeHppFile_32bits COMMAND  $<TARGET_FILE:bin2cpp> ${CMAKE_CURRENT_SOURCE_DIR}/SourceCode.hpp -s -b 32) 
add_test(NAME FromSourceCodeHppFile_64bits COMMAND  $<TARGET_FILE:bin2cpp> ${CMAKE_CURRENT_SOURCE_DIR}/SourceCode.hpp -s -b 64) 
