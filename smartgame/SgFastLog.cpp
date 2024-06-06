//----------------------------------------------------------------------------
/** @file SgFastLog.cpp */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgFastLog.h"

#include <limits>
#include <cmath>

using namespace std;

//----------------------------------------------------------------------------

namespace {

// The fast log algorithm requires 4 byte integers
static_assert(sizeof(int) == 4);

// The fast log algorithm requires that floats use IEEE 754 format
static_assert(numeric_limits<float>::is_iec559);

} // namespace

//----------------------------------------------------------------------------

SgFastLog::SgFastLog(int mantissaBits)
    : m_mantissaBitsDiff(MAX_MANTISSA_BITS - mantissaBits)
{
    m_lookupTable = new float[(int)(1 << mantissaBits)];
    IntFloat x;
    x.m_int = 0x3F800000;
    int incr = (1 << m_mantissaBitsDiff);
    int p = static_cast<int>(pow(2.0f, mantissaBits));
    float invLogTwo = 1.f / log(2.f);
    for (int i = 0; i < p; ++i)
    {
        m_lookupTable[i] = log(x.m_float) * invLogTwo;
        x.m_int += incr;
    }
}

SgFastLog::~SgFastLog()
{
    delete[] m_lookupTable;
}

//----------------------------------------------------------------------------
