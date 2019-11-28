#pragma once

#include "SDL_lava.h"

typedef enum{
	VK_CMD_BEGIN_RENDER_PASS,
	VK_CMD_BIND_PIPELINE,
	VK_CMD_BIND_VERTEX_BUFFER,
	VK_CMD_BIND_INDEX_BUFFER,
	VK_CMD_BIND_DESCRIPTOR_SET,
	VK_CMD_DRAW,
	VK_CMD_DRAW_INDEXED,
	VK_CMD_END_RENDER_PASS
}cmd_t;

typedef struct{
	void (*p)(VkCommandBuffer, VkPipelineBindPoint, VkPipeline);
	VkPipelineBindPoint bind_point;
	VkPipeline pipeline;
}cmd_bind_pipeline_t;

typedef struct {
	void (*p)(VkCommandBuffer, uint32_t, uint32_t, const VkBuffer*, const VkDeviceSize*);
	uint32_t first_binding;
	uint32_t bindings_count;
	const VkBuffer* buffers;
    const VkDeviceSize* offsets;
}cmd_bind_vertex_buffers_t;

typedef struct{
	void (*p)(VkCommandBuffer, VkBuffer, VkDeviceSize, VkIndexType);
	VkBuffer buffer;
    VkDeviceSize offset;
    VkIndexType index_type;
}cmd_bind_index_buffers_t;

typedef struct{
	void (*p)(VkCommandBuffer, VkPipelineBindPoint, VkPipelineLayout, uint32_t, uint32_t, const VkDescriptorSet*, uint32_t, const uint32_t*);
	VkPipelineBindPoint  pipeline_bind_point;
    VkPipelineLayout layout;
    uint32_t first_set;
    uint32_t descriptors_count;
    const VkDescriptorSet* descriptor_sets;
    uint32_t offset_count;
    const uint32_t* offsets;
}cmd_bind_descriptor_sets_t;

typedef struct{
	void (*p)(VkCommandBuffer, uint32_t, uint32_t, uint32_t, int32_t, uint32_t);
	uint32_t index_count;
	uint32_t instance_count;
	uint32_t first_index;
	int32_t vertex_offset;
	uint32_t first_instance;
}cmd_draw_indexed_t;

typedef struct{
	void (*p)(VkCommandBuffer, uint32_t, uint32_t, uint32_t, uint32_t);
	uint32_t vertex_count;
	uint32_t instance_count;
	uint32_t first_vertex;
	uint32_t firstInstance;
}cmd_draw_t;

typedef union{
	cmd_bind_pipeline_t bind_pipeline;
	cmd_bind_vertex_buffers_t bind_vertex_buffers;
	cmd_bind_index_buffers_t bind_index_buffers;
	cmd_bind_descriptor_sets_t bind_descriptor_sets;
	cmd_draw_indexed_t draw_indexed;
	cmd_draw_t draw;
}cmd_u;

typedef struct{
	cmd_t type;
	cmd_u uni;
}command_t;

command_t VK_BindVertexBuffer(uint32_t first_binding, uint32_t bindings_count, const VkBuffer* buffers, const VkDeviceSize* offsets);
command_t VK_BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType index_type);


void VK_CreateCommandBuffers(VK_Context *ctx, uint32_t count, command_t *cmd);
