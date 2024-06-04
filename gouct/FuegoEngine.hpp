//
// Created by Alexander Pototskiy on 03.06.24.
//

#pragma once

#include <string>
#include <string_view>
#include <utility>
#include <optional>

#include "GoGtpEngine.h"

struct fuego_engine_configuration
{
    std::string programPath;
    std::string configPath;
    int srand;
    int fixedBoardSize;
    int maxGames;

    /** Use opening book */
    bool useBook = true;
    bool allowHandicap = true;
};

class FuegoEngine
{
public:
    explicit FuegoEngine(fuego_engine_configuration const&);
    FuegoEngine(FuegoEngine const&) = delete;
    FuegoEngine& operator=(FuegoEngine const&) = delete;
    ~FuegoEngine();
    
    GoGtpEngine& engine() { return *impl_; }

    // returns {success, result}
    std::pair<bool, std::string> ProcessCommand(std::string_view command) noexcept;

private:
    std::unique_ptr<GoGtpEngine> impl_;
    std::optional<GoGtpAssertionHandler> assertionHandler_;
};

extern "C" {

struct fuego_result_descriptor {
    void* result;
    int success;
};

fuego_result_descriptor fuego_create_engine(
    const char* program_path,
    const char* config_path,
    int srand,
    int fixed_board_size,
    int max_games,
    int use_book,
    int allow_handicap);

void fuego_free_string(void* cookie);
void fuego_free_engine(void* cookie);

fuego_result_descriptor fuego_process_command(void* cookie, const char* cmd, size_t cmdlen);

}
