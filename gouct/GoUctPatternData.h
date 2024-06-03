//----------------------------------------------------------------------------
/** @file GoUctPatternData.h
    Data structures for pattern data arrays.
*/
//----------------------------------------------------------------------------

#pragma once

#include "SgBWArray.h"

namespace GoUctPatternData {

typedef struct
{
    int m_code;
    double m_value;
} PatternTableEntry;

typedef struct
{
	int m_nuPatterns;
    const PatternTableEntry* m_patternArray;
} PatternTable;

typedef SgBWArray<PatternTable> BWTable;

typedef struct
{
	BWTable m_edgePatterns;
	BWTable m_centerPatterns;
} PatternData;

} // namespace GoUctPatternData
