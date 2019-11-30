#pragma once

#include "SDL_lava.h"



void VK_CreateTextureImage(VK_Context *ctx, VK_Texture *tex, const char *file);
void VK_CreateTextureImageView(VK_Context *ctx, VK_Texture *tex);
void VK_CreateTextureSampler(VK_Context *ctx, VK_Texture *tex);

void VK_LoadModel(VK_Context *ctx, const char* path);
