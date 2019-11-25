#pragma once

#include "SDL_lava.h"

uint32_t VK_GetMemoryType(VK_Context *ctx, uint32_t type_filter, VkMemoryPropertyFlags properties);
void VK_CreateBuffer(VK_Context *ctx, VkBuffer *buffer, VkDeviceMemory *data, VkDeviceSize size , VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
void VK_CreateVertexBuffer(VK_Context *ctx, mesh_t *mesh);
void VK_CreateIndexBuffer(VK_Context *ctx, mesh_t *mesh);
void VK_CreateUniformBuffer(VK_Context *ctx);
void VK_CopyBuffer(VK_Context *ctx, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) ;
