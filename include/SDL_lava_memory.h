#pragma once

#include "SDL_lava.h"

uint32_t VK_GetMemoryType(VK_Renderer *renderer, uint32_t type_filter, VkMemoryPropertyFlags properties);
void VK_CreateBuffer(VK_Renderer *renderer, VkBuffer *buffer, VkDeviceMemory *data, VkDeviceSize size , VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
void VK_CreateVertexBuffer(VK_Renderer *renderer, mesh_t *mesh);
void VK_CreateIndexBuffer(VK_Renderer *renderer, mesh_t *mesh);
void VK_CreateUniformBuffer(VK_Renderer *renderer);
void VK_CopyBuffer(VK_Renderer *renderer, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) ;
