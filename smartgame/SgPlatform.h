//----------------------------------------------------------------------------
/** @file SgPlatform.h */
//----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <filesystem>

//----------------------------------------------------------------------------

/** Get information about the current computer. */
namespace SgPlatform
{
    namespace fs = std::filesystem;

    /** @see SetProgramDir */
    const fs::path& GetProgramDir();

    /** @see SetTopSourceDir */
    const fs::path& GetTopSourceDir();

    /** Set the program directory. Should be called by main(). */
    void SetProgramDir(const fs::path& dir);

    /** Set the top-level source directory (fuego). */
    void SetTopSourceDir(const fs::path& dir);

    /** Get total amount of memory available on the system.
        @return The total memory in bytes or 0 if the memory cannot be
        determined. */
    size_t TotalMemory();

}

//----------------------------------------------------------------------------
