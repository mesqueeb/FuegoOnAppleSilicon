//
// Created by Alexander Pototskiy on 03.06.24.
//

#pragma once

#include <string>

#include <utility>

//inline int cxxFunction(int n) { return n + 1; }

struct FuegoEngine
{
public:
    FuegoEngine();
    //FuegoEngine(FuegoEngine const&) = delete;
    //FuegoEngine& operator=(FuegoEngine const&) = delete;
    ~FuegoEngine();
    
    // returns {success, result}
    std::pair<bool, std::string> ProcessCommand(std::string const& command) noexcept;
};
