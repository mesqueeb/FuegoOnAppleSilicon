include_directories(../gtpengine)
include_directories(../smartgame)
include_directories(../go)
include_directories(../gouct)
include_directories(../simpleplayers)

set (EXE_NAME fuego_test)

set (EXE_SOURCES
        FuegoTestEngine.cpp
        FuegoTestMain.cpp
)

add_executable(${EXE_NAME} ${EXE_SOURCES})

target_link_libraries(${EXE_NAME} fuego_gouct)
target_link_libraries(${EXE_NAME} fuego_simpleplayers)
