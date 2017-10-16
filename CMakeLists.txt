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
set(CATCH_INCLUDE_DIR ${CATCH_INCLUDE_DIR} PARENT_SCOPE)
set(RANGEV3_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/range-v3/include)
set(RANGEV3_INCLUDE_DIR ${RANGEV3_INCLUDE_DIR} PARENT_SCOPE)
set(FMT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt)
set(FMT_INCLUDE_DIR ${FMT_INCLUDE_DIR} PARENT_SCOPE)
set(JSON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/json/src)
set(JSON_INCLUDE_DIRS ${JSON_INCLUDE_DIRS} PARENT_SCOPE)
set(QTL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/qtl/include)
set(QTL_INCLUDE_DIRS ${QTL_INCLUDE_DIRS} PARENT_SCOPE)

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

cpr_option(BUILD_OANDA_EXEC "Set to ON to build oanda exec." ON)
cpr_option(BUILD_OANDA_TESTS "Set to ON to build oanda tests." ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")

if(BUILD_OANDA_EXEC)
  add_executable(oanda_exec main.cc)
  target_link_libraries(oanda_exec ${CPR_LIBRARIES})
  target_link_libraries(oanda_exec ${MYSQL_LIBS})
endif()

if(BUILD_OANDA_TESTS)
  add_executable(oanda_tests test/tests.cc)
  target_link_libraries(oanda_tests ${CPR_LIBRARIES})
  target_link_libraries(oanda_tests ${MYSQL_LIBS})

  enable_testing()
  add_test(NAME Tests COMMAND oanda_tests)
endif()

if(BUILD_OANDA_EXEC AND BUILD_OANDA_TESTS)
  add_dependencies(oanda_exec oanda_tests)
endif()

