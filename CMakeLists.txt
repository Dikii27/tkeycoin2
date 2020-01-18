cmake_minimum_required(VERSION 3.10)
project(tkeycoin_ng)

set(CMAKE_CXX_STANDARD 17)

option(WITH_TESTS "Build test (over gtest)" ON)

# Add path for custom modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# If ccache is available, then use it.
find_program(CCACHE ccache)
if(CCACHE)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE})
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE})
endif(CCACHE)

# Use OpenSSL
include(BrewHelper)
find_brew_prefix(OPENSSL_ROOT_DIR openssl)
find_package(OpenSSL REQUIRED)

include_directories(src)
include_directories(src/other/secp256k1)

file(GLOB_RECURSE CPP_FILES src/*.cpp src/*.c)
file(GLOB_RECURSE TEST_FILES src/*_test.cpp)
file(GLOB_RECURSE AUX_FILES main.cpp dummy.cpp test.cpp)

message(STATUS "Aux files:")
foreach(F ${AUX_FILES})
	message(${F})
	list(REMOVE_ITEM CPP_FILES ${F})
	list(REMOVE_ITEM TEST_FILES ${F})
endforeach()

message(STATUS "Test files:")
foreach(F ${TEST_FILES})
	message(${F})
	list(REMOVE_ITEM CPP_FILES ${F})
endforeach()

message(STATUS "Source files:")
foreach(F ${CPP_FILES})
	message(${F})
endforeach()

add_library(core ${CPP_FILES})

add_subdirectory(lib/secp256k1)
include_directories(lib/secp256k1/include)
include_directories(lib/secp256k1/contrib)

add_subdirectory(lib/primitive)
include_directories(${PRIMITIVE_INCLUDES})

target_link_libraries(core
	secp256k1
	OpenSSL::SSL
)

if (WITH_TESTS)
	message(STATUS "Testing is enabled")

	find_package(GTest REQUIRED)

	# If find_package reports about libraries not found, just use follow command:
	#  cd /usr/src/gtest
	#  sudo cmake CMakeLists.txt
	#  sudo make
	#  sudo ln -s /usr/src/gtest/*.a /usr/lib/
	enable_testing()

	include_directories(${GTEST_INCLUDE_DIRS})

	add_executable(tests ${TEST_FILES})

	target_link_libraries(tests core pthread ${GTEST_BOTH_LIBRARIES})
endif()

#add_library(tkey_main src/main.cpp)
add_executable(tkey src/main.cpp ${PRIMITIVE_OBJECTS})
target_link_libraries(tkey core primitive_static)# tkey_main)

#add_library(tkey_main src/main.cpp)
#add_executable(tkey src/dummy.cpp)
#target_link_libraries(tkey core primitive_static tkey_main)
