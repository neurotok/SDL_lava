#pragma once

#include "SDL_lava.h"



void LAV_CreateTextureImage(LAV_Context *ctx, LAV_Texture *tex, const char *file);
void LAV_CreateTextureImageView(LAV_Context *ctx, LAV_Texture *tex);
void LAV_CreateTextureSampler(LAV_Context *ctx, LAV_Texture *tex);

void LAV_LoadModel(LAV_Context *ctx, const char* path);
