include_directories(../gtpengine)
include_directories(../smartgame)
include_directories(../go)

set (LIBRARY_NAME fuego_gouct)

set (LIBRARY_SOURCES
        GoUctAdditiveKnowledge.cpp
        GoUctAdditiveKnowledgeFuego.cpp
        GoUctAdditiveKnowledgeGreenpeep.cpp
        GoUctAdditiveKnowledgeMultiple.cpp
        GoUctBoard.cpp
        GoUctCommands.cpp
        GoUctDefaultPriorKnowledge.cpp
        GoUctDefaultMoveFilter.cpp
        GoUctEstimatorStat.cpp
        GoUctGlobalSearch.cpp
        GoUctKnowledge.cpp
        GoUctKnowledgeFactory.cpp
        GoUctLadderKnowledge.cpp
        GoUctObjectWithSearch.cpp
        GoUctPlayoutPolicy.cpp
        GoUctMoveFilter.cpp
        GoUctSearch.cpp
        GoUctUtil.cpp
)

add_library(${LIBRARY_NAME} STATIC ${LIBRARY_SOURCES})

target_link_libraries(${LIBRARY_NAME} fuego_go)
