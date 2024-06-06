include_directories(../gtpengine)
include_directories(../smartgame)
include_directories(../go)

set (LIBRARY_NAME fuego_simpleplayers)

set (LIBRARY_SOURCES
        SpAveragePlayer.cpp
        SpCapturePlayer.cpp
        SpDumbTacticalPlayer.cpp
        SpGreedyPlayer.cpp
        SpInfluencePlayer.cpp
        SpLadderPlayer.cpp
        SpLibertyPlayer.cpp
        SpMaxEyePlayer.cpp
        SpMinLibPlayer.cpp
        SpMoveGenerator.cpp
        SpRandomPlayer.cpp
        SpSafePlayer.cpp
        SpSimplePlayer.cpp
        SpUtil.cpp
)

add_library(${LIBRARY_NAME} STATIC ${LIBRARY_SOURCES})

target_link_libraries(${LIBRARY_NAME} fuego_go)
