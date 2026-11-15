#ifndef COUNT_CHANGES_H
#define COUNT_CHANGES_H
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "streams.h"

// Append session, generation and per-key changes to an open JSON object.
// Unknown/reset/missing baselines are JSON null, never guessed as zero.
void append_count_changes(Stream *, const char *path,
    const std::vector<std::pair<std::string, uint64_t>> &, uint64_t generation = 0);
#endif
