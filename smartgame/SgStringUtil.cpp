//----------------------------------------------------------------------------
/** @file SgStringUtil.cpp
    See SgStringUtil.h */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgStringUtil.h"

#include <cctype>
#include <sstream>

//----------------------------------------------------------------------------
std::string SgStringUtil::GetNativeFileName(const fs::path& file)
{
    fs::path normalizedFile = file.lexically_normal();
    return normalizedFile.string();
}

std::vector<std::string> SgStringUtil::SplitArguments(std::string_view s)
{
    std::vector<std::string> result;
    bool escape = false;
    bool inString = false;
    std::ostringstream token;
    for (size_t i = 0; i < s.size(); ++i)
    {
        char c = s[i];
        if (c == '"' && ! escape)
        {
            if (inString)
            {
                result.push_back(token.str());
                token.str("");
            }
            inString = ! inString;
        }
        else if (isspace(c) && ! inString)
        {
            if (! token.str().empty())
            {
                result.push_back(token.str());
                token.str("");
            }
        }
        else
            token << c;
        escape = (c == '\\' && ! escape);
    }
    if (! token.str().empty())
        result.push_back(token.str());
    return result;
}

//----------------------------------------------------------------------------
