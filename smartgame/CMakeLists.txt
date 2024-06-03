include_directories(../gtpengine)

set (LIBRARY_NAME fuego_smartgame)

set (LIBRARY_SOURCES
        SgBoardConst.cpp
        SgBookBuilder.cpp
        SgCmdLineOpt.cpp
        SgConnCompIterator.cpp
        SgDebug.cpp
        SgDfpnSearch.cpp
        SgEvaluatedMoves.cpp
        SgException.cpp
        SgFastLog.cpp
        SgGameReader.cpp
        SgGameWriter.cpp
        SgGtpClient.cpp
        SgGtpCommands.cpp
        SgGtpUtil.cpp
        SgIncrementalStack.cpp
        SgInit.cpp
        SgMemCheck.cpp
        SgMiaiMap.cpp
        SgMiaiStrategy.cpp
        SgNbIterator.cpp
        SgNode.cpp
        SgNodeUtil.cpp
        SgPoint.cpp
        SgPointSet.cpp
        SgPointSetUtil.cpp
        SgProbCut.cpp
#        SgProcess.cpp
        SgProp.cpp
        SgRandom.cpp
        SgRect.cpp
        SgSearch.cpp
        SgSearchControl.cpp
        SgSearchStatistics.cpp
        SgSearchTracer.cpp
        SgSearchValue.cpp
        SgStrategy.cpp
        SgStringUtil.cpp
        SgMpiSynchronizer.cpp
        SgPlatform.cpp
        SgSystem.cpp
        SgTime.cpp
        SgTimeControl.cpp
        SgTimeRecord.cpp
        SgUctSearch.cpp
        SgUctTree.cpp
        SgUctTreeUtil.cpp
        SgUtil.cpp
        SgVectorUtil.cpp
        SgWrite.cpp
)

add_library(${LIBRARY_NAME} STATIC ${LIBRARY_SOURCES})

target_link_libraries(${LIBRARY_NAME} fuego_gtpengine)
