include_directories(../gtpengine)
include_directories(../smartgame)

set (LIBRARY_NAME fuego_go)

set (LIBRARY_SOURCES
    GoAutoBook.cpp
    GoBensonSolver.cpp
    GoBlock.cpp
    GoBoard.cpp
    GoBoardCheckPerformance.cpp
    GoBoardHistory.cpp
    GoBoardRestorer.cpp
    GoBoardSynchronizer.cpp
    GoBoardUpdater.cpp
    GoBoardUtil.cpp
    GoBook.cpp
    GoChain.cpp
    GoEyeCount.cpp
    GoEyeUtil.cpp
    GoGame.cpp
    GoGtpCommandUtil.cpp
    GoGtpEngine.cpp
    GoGtpExtraCommands.cpp
    GoInfluence.cpp
    GoInit.cpp
    GoKomi.cpp
    GoLadder.cpp
    GoMotive.cpp
    GoNodeUtil.cpp
    GoPlayer.cpp
    GoPlayerMove.cpp
    GoRegion.cpp
    GoRegionBoard.cpp
    GoRegionUtil.cpp
    GoRules.cpp
    GoSafetyCommands.cpp
    GoSafetySolver.cpp
    GoSafetyUtil.cpp
    GoSearch.cpp
    GoSetupUtil.cpp
    GoStaticLadder.cpp
    GoStaticSafetySolver.cpp
    GoTimeControl.cpp
    GoTimeSettings.cpp
    GoUtil.cpp
)

add_library(${LIBRARY_NAME} STATIC ${LIBRARY_SOURCES})

target_link_libraries(${LIBRARY_NAME} fuego_smartgame)
