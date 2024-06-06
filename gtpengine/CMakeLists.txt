set (LIBRARY_NAME fuego_gtpengine)

set (LIBRARY_SOURCES
    GtpEngine.cpp
    GtpInputStream.cpp
    GtpOutputStream.cpp
)

add_library(${LIBRARY_NAME} STATIC ${LIBRARY_SOURCES})
