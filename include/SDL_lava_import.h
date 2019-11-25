#pragma once

#include "SDL_lava.h"

void VK_CreateTextureImage(VK_Renderer *renderer, const char *file);
void VK_CreateTextureImageView(VK_Renderer *renderer);
void VK_CreateTextureSampler(VK_Renderer *renderer);
void VK_LoadModel(VK_Renderer *renderer, const char* path);
