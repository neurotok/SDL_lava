#pragma once

#include "SDL_lava.h"

uint32_t LAV_GetMemoryType(LAV_Context *ctx, uint32_t type_filter, VkMemoryPropertyFlags properties);
void LAV_CreateBuffer(LAV_Context *ctx, VkBuffer *buffer, VkDeviceMemory *data, VkDeviceSize size , VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
void LAV_CreateVertexBuffer(LAV_Context *ctx, mesh_t *mesh);
void LAV_CreateIndexBuffer(LAV_Context *ctx, mesh_t *mesh);
//void LAV_CreateUniformBuffer(LAV_Context *ctx, LAV_UniformBuffer *ubo);

void LAV_CopyBuffer(LAV_Context *ctx, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) ;
