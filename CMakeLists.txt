cmake_minimum_required (VERSION 3.12)
project (FuegoOnAppleSilicon)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (DEFINED BOOST_INCLUDE)
    message("Boost include path: ${BOOST_INCLUDE}")
    include_directories(${BOOST_INCLUDE})
	#set (PROJECT_NAME "$ENV{PROJECT_NAME}")
endif()

if (DEFINED BOOST_LIB)
    link_directories(${BOOST_LIB})
endif()

if (DEFINED CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}")
endif()

set (BOOST_SUFFIX "-vc143-mt-gd-x64-1_85")

set (BOOST_UNIT_TEST_FRAMEWORK_LIB boost_unit_test_framework${BOOST_SUFFIX})


message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

add_subdirectory(gtpengine)
add_subdirectory(smartgame)
add_subdirectory(go)
add_subdirectory(gouct)
add_subdirectory(fuegomain)
add_subdirectory(simpleplayers)
add_subdirectory(fuegotest)
add_subdirectory(unittestmain)