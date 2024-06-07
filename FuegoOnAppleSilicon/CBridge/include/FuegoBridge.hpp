#ifndef Bridging_Header_h
#define Bridging_Header_h
#include <stdlib.h>

struct fuego_result_descriptor {
    void* result;
    int success;
};

struct fuego_result_descriptor fuego_create_engine(
    const char* program_path,
    const char* config_path,
    int srand,
    int fixed_board_size,
    int max_games,
    int use_book,
    int allow_handicap);

void fuego_free_string(void* cookie);
void fuego_free_engine(void* cookie);

struct fuego_result_descriptor fuego_process_command(void* cookie, const char* cmd, uint64_t cmdlen);

#endif
