//----------------------------------------------------------------------------
/** @file SgDebug.cpp
    See SgDebug.h */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgDebug.h"

#include <fstream>
#include <iostream>
#include <memory>

//----------------------------------------------------------------------------

/** Null stream.
    This file stream will never be opened and acts as a null stream
    for SgDebug(). */
static std::ofstream s_nullStream;

static std::unique_ptr<std::ofstream> s_fileStream;


std::ostream* g_debugStrPtr(&std::cerr);

std::ostream& SgDebug()
{
    if (! g_debugStrPtr->good())
    {
        // does not just use a direct SG_ASSERT(g_debugStrPtr->good())
        // in order to allow a breakpoint to be set on the line below.
        SG_ASSERT(false);
    }
    return *g_debugStrPtr;
}

std::ostream& SgWarning()
{
    SgDebug() << "WARNING: ";
    return SgDebug();
}

//----------------------------------------------------------------------------

void SgDebugToWindow()
{
    g_debugStrPtr = &std::cerr;
}

void SgDebugToFile(const char* filename)
{
    if (s_fileStream.get() == 0)
        s_fileStream.reset(new std::ofstream(filename, std::ios::app));
    g_debugStrPtr = s_fileStream.get();
}

void SgDebugToNull()
{
    g_debugStrPtr = &s_nullStream;
}

std::ostream* SgSwapDebugStr(std::ostream* newStr)
{
    std::ostream* t = g_debugStrPtr;
    g_debugStrPtr = newStr;
    return t;
}

//----------------------------------------------------------------------------

SgDebugToNewFile::SgDebugToNewFile(const char* filename)
    : m_old(SgSwapDebugStr(new std::ofstream(filename, std::ios::app)))
{ }

SgDebugToNewFile::SgDebugToNewFile()
    : m_old(NULL)
{ }

void SgDebugToNewFile::SetFile(const char* filename)
{
    m_old = SgSwapDebugStr(new std::ofstream(filename, std::ios::app));
}

SgDebugToNewFile::~SgDebugToNewFile()
{
    if (m_old)
    {
        std::ostream* t = SgSwapDebugStr(m_old);
        delete t;
    }
}

//----------------------------------------------------------------------------

SgDebugToString::SgDebugToString(bool writeToOldDebugStr)
    : m_writeToOldDebugStr(writeToOldDebugStr)
{
    m_old = SgSwapDebugStr(&m_str);
}

SgDebugToString::~SgDebugToString()
{
    if (m_writeToOldDebugStr)
        (*m_old) << GetString();
    SgSwapDebugStr(m_old);
}

//----------------------------------------------------------------------------

