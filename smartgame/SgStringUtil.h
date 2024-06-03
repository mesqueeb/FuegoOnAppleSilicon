//----------------------------------------------------------------------------
/** @file SgStringUtil.h */
//----------------------------------------------------------------------------

#pragma once
#include <string>
#include <filesystem>
#include <vector>

//----------------------------------------------------------------------------

/** String utility functions. */
namespace SgStringUtil
{
    namespace fs = std::filesystem;

	/** Convert generic into native file name */
    std::string GetNativeFileName(
    	const fs::path& file);
	
    /** Split command line into arguments.
        Allows " for words containing whitespaces. */
    std::vector<std::string> SplitArguments(std::string_view s);
}

//----------------------------------------------------------------------------
