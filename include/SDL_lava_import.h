#pragma once

#include "SDL_lava.h"

void VK_CreateTextureImage(VK_Context *ctx, const char *file);
void VK_CreateTextureImageView(VK_Context *ctx);
void VK_CreateTextureSampler(VK_Context *ctx);
void VK_LoadModel(VK_Context *ctx, const char* path);
