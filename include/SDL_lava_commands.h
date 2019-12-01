#pragma once

#include "SDL_lava.h"

typedef enum{
	LAV_CMD_BEGIN_RENDER_PASS,
	LAV_CMD_BIND_PIPELINE,
	LAV_CMD_BIND_VERTEX_BUFFER,
	LAV_CMD_BIND_INDEX_BUFFER,
	LAV_CMD_BIND_DESCRIPTOR_SET,
	LAV_CMD_DRAW,
	LAV_CMD_DRAW_INDEXED,
	LAV_CMD_END_RENDER_PASS
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
	uint32_t first_instance;
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
}LavCommand;

LavCommand LAV_BindPipeline(VkPipelineBindPoint bind_point, VkPipeline pipeline);
LavCommand LAV_BindVertexBuffer(uint32_t first_binding, uint32_t bindings_count, const VkBuffer* buffers, const VkDeviceSize* offsets);
LavCommand LAV_BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType index_type);
LavCommand LAV_BindDescriptors(VkPipelineBindPoint  pipeline_bind_point,  VkPipelineLayout layout, uint32_t first_set, uint32_t descriptors_count, const VkDescriptorSet* descriptor_sets, uint32_t offset_count, const uint32_t* offsets);
LavCommand LAV_DrawIndexed(uint32_t index_count, uint32_t instance_count,	uint32_t first_index, int32_t vertex_offset, uint32_t first_instance);
LavCommand LAV_Draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);

void LAV_ExecuteCommands(VkCommandBuffer command_buffer, VkDescriptorSet *descriptor_sets, uint32_t count, LavCommand *cmd);
void LAV_CreateCommandBuffers(LAV_Context *ctx, uint32_t count, LavCommand *cmd);
