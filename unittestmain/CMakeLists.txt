include_directories(../gtpengine)
include_directories(../smartgame)
include_directories(../go)
include_directories(../gouct)
include_directories(../simpleplayers)

set (EXE_NAME fuego_unittest)

set (EXE_SOURCES
        ../go/test/GoBoardTest.cpp
        ../go/test/GoBoardSynchronizerTest.cpp
        ../go/test/GoBoardUpdaterTest.cpp
        ../go/test/GoBoardUtilTest.cpp
        ../go/test/GoBookTest.cpp
        ../go/test/GoEyeUtilTest.cpp
        ../go/test/GoGameTest.cpp
        ../go/test/GoGtpCommandUtilTest.cpp
        ../go/test/GoGtpEngineTest.cpp
        ../go/test/GoKomiTest.cpp
        ../go/test/GoLadderTest.cpp
        ../go/test/GoRegionTest.cpp
        ../go/test/GoRegionBoardTest.cpp
        ../go/test/GoSetupUtilTest.cpp
        ../go/test/GoTimeControlTest.cpp
        ../go/test/GoTimeSettingsTest.cpp
        ../go/test/GoUtilTest.cpp
        ../gouct/test/GoUctAdditiveKnowledgeMultipleTest.cpp
        ../gouct/test/GoUctBoardTest.cpp
        ../gouct/test/GoUctKnowledgeTest.cpp
        ../gouct/test/GoUctLadderKnowledgeTest.cpp
        ../gouct/test/GoUctUtilTest.cpp
        ../gtpengine/test/GtpEngineTest.cpp
        ../smartgame/test/SgArrayTest.cpp
        ../smartgame/test/SgArrayListTest.cpp
        ../smartgame/test/SgBlackWhiteTest.cpp
        ../smartgame/test/SgBoardColorTest.cpp
        ../smartgame/test/SgBoardConstTest.cpp
        ../smartgame/test/SgBWArrayTest.cpp
        ../smartgame/test/SgBWSetTest.cpp
        ../smartgame/test/SgCmdLineOptTest.cpp
        ../smartgame/test/SgConnCompIteratorTest.cpp
        ../smartgame/test/SgEBWArrayTest.cpp
        ../smartgame/test/SgEvaluatedMovesTest.cpp
        ../smartgame/test/SgFastLogTest.cpp
        ../smartgame/test/SgGameReaderTest.cpp
        ../smartgame/test/SgGtpUtilTest.cpp
        ../smartgame/test/SgHashTest.cpp
        ../smartgame/test/SgMarkerTest.cpp
        ../smartgame/test/SgMathTest.cpp
        ../smartgame/test/SgMiaiMapTest.cpp
        ../smartgame/test/SgMiaiStrategyTest.cpp
        ../smartgame/test/SgNbIteratorTest.cpp
        ../smartgame/test/SgNodeTest.cpp
        ../smartgame/test/SgNodeUtilTest.cpp
        ../smartgame/test/SgPointArrayTest.cpp
        ../smartgame/test/SgPointTest.cpp
        ../smartgame/test/SgPointSetTest.cpp
        ../smartgame/test/SgPropTest.cpp
        ../smartgame/test/SgRandomTest.cpp
        ../smartgame/test/SgRectTest.cpp
        ../smartgame/test/SgRestorerTest.cpp
        ../smartgame/test/SgSearchTest.cpp
        ../smartgame/test/SgSortedArrayTest.cpp
        ../smartgame/test/SgSortedMovesTest.cpp
        ../smartgame/test/SgStackTest.cpp
        ../smartgame/test/SgStatisticsTest.cpp
        ../smartgame/test/SgStringUtilTest.cpp
        ../smartgame/test/SgSystemTest.cpp
        ../smartgame/test/SgTimeControlTest.cpp
        ../smartgame/test/SgUctSearchTest.cpp
        ../smartgame/test/SgUctTreeTest.cpp
        ../smartgame/test/SgUctTreeUtilTest.cpp
        ../smartgame/test/SgUctValueTest.cpp
        ../smartgame/test/SgUtilTest.cpp
        ../smartgame/test/SgVectorTest.cpp
        ../smartgame/test/SgVectorUtilTest.cpp
        ../smartgame/test/SgWriteTest.cpp
        ../unittestmain/UnitTestMain.cpp
)

add_executable(${EXE_NAME} ${EXE_SOURCES})

target_link_libraries(${EXE_NAME} fuego_gouct)
target_link_libraries(${EXE_NAME} fuego_simpleplayers)

#used included
#add_framework(${EXE_NAME} boost_unit_test_framework ${BOOST_HOME})
