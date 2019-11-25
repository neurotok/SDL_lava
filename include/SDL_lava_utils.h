#pragma once

#include <stdio.h>
#include <stdint.h>

int clamp(int32_t i, int32_t min, int32_t max);
size_t file_get_lenght(FILE *file);
//const char* get_file_data(size_t* len, const char* filename);
