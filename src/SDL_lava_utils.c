#include "SDL_lava_utils.h"

int clamp(int32_t i, int32_t min, int32_t max){
	if (i < min)
		return min;
	if (i > max)
		return max;
	return i;
}

size_t file_get_lenght(FILE *file){

	size_t lenght;
	size_t currPos = ftell(file);
	fseek(file, 0 , SEEK_END);
	lenght = ftell(file);
	fseek(file, currPos, SEEK_SET);
	return lenght;
}


