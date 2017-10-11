cmake_minimum_required(VERSION 3.1)
project(oanda-trading)

set(CMAKE_VERBOSE_MAKEFILE ON)

execute_process(
  COMMAND mysql_config --include
  OUTPUT_VARIABLE MYSQL_INCLUDE_DIRS
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

execute_process(
  COMMAND mysql_config --libs
  OUTPUT_VARIABLE MYSQL_LIBS
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

string(REGEX REPLACE "-I" "" MYSQL_INCLUDE_DIRS "${MYSQL_INCLUDE_DIRS}")

set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/catch/single_include)
set(RANGEV3_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/range-v3/include)
set(FMT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt)
set(JSON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/json/src)
set(QTL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/qtl/include)

if(WIN32)
    option(CMAKE_USE_OPENSSL "Use OpenSSL code. Experimental" OFF)
endif()

set(BUILD_CPR_TESTS OFF CACHE BOOL "Don't build cpr tests")
find_package(CURL)
if(CURL_FOUND)
    set(USE_SYSTEM_CURL ON CACHE BOOL "Use the system curl for faster builds")
endif()
add_subdirectory(include/cpr)

include_directories(
  ${CATCH_INCLUDE_DIR}
  ${RANGEV3_INCLUDE_DIR}
  ${FMT_INCLUDE_DIR}
  ${CPR_INCLUDE_DIRS}
  ${JSON_INCLUDE_DIRS}
  ${QTL_INCLUDE_DIRS}
  ${MYSQL_INCLUDE_DIRS}
  )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")
add_executable(main main.cc)
target_link_libraries(main ${CPR_LIBRARIES})
target_link_libraries(main ${MYSQL_LIBS})

add_executable(tests test/tests.cc)
target_link_libraries(tests ${CPR_LIBRARIES})
add_dependencies(main tests)

enable_testing()
add_test(NAME Tests COMMAND tests)
