include_directories(../gtpengine)
include_directories(../smartgame)
include_directories(../go)
include_directories(../gouct)

set (EXE_NAME fuego)

set (EXE_SOURCES
        FuegoMain.cpp
        #FuegoMainEngine.cpp
        FuegoMainUtil.cpp
)

add_executable(${EXE_NAME} ${EXE_SOURCES})

target_link_libraries(${EXE_NAME} fuego_gouct)

add_framework(${EXE_NAME} boost_program_options ${BOOST_HOME})
