#include "SDL_lava_commands.h"

#define NUM(a) (sizeof(a)/sizeof(a[0]))

LavCommand LAV_BindPipeline(VkPipelineBindPoint bind_point, VkPipeline pipeline){
	
	LavCommand cmd;

	cmd.type = LAV_CMD_BIND_PIPELINE;

	cmd.uni.bind_pipeline.p = &vkCmdBindPipeline; 
	cmd.uni.bind_pipeline.bind_point = bind_point;
	cmd.uni.bind_pipeline.pipeline = pipeline;

	return cmd;
}

LavCommand LAV_BindVertexBuffer(uint32_t first_binding, uint32_t bindings_count, const VkBuffer* buffers, const VkDeviceSize* offsets){

	LavCommand cmd;

	cmd.type = LAV_CMD_BIND_VERTEX_BUFFER;

	cmd.uni.bind_vertex_buffers.p = &vkCmdBindVertexBuffers;
	cmd.uni.bind_vertex_buffers.first_binding = first_binding;
	cmd.uni.bind_vertex_buffers.bindings_count = bindings_count;
	cmd.uni.bind_vertex_buffers.buffers = buffers;
	cmd.uni.bind_vertex_buffers.offsets = offsets;

	return cmd;
}

LavCommand LAV_BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType index_type){
	
	LavCommand cmd;

	cmd.type = LAV_CMD_BIND_INDEX_BUFFER;

	cmd.uni.bind_index_buffers.p = &vkCmdBindIndexBuffer; 
	cmd.uni.bind_index_buffers.buffer = buffer;
	cmd.uni.bind_index_buffers.offset = offset;
	cmd.uni.bind_index_buffers.index_type = index_type;

	return cmd;
}

LavCommand LAV_BindDescriptors(VkPipelineBindPoint  pipeline_bind_point,  VkPipelineLayout layout, uint32_t first_set, uint32_t descriptors_count, const VkDescriptorSet* descriptor_sets, uint32_t offset_count, const uint32_t* offsets){
	
	LavCommand cmd;

	cmd.type = LAV_CMD_BIND_DESCRIPTOR_SET;

	cmd.uni.bind_descriptor_sets.p = &vkCmdBindDescriptorSets;
	cmd.uni.bind_descriptor_sets.pipeline_bind_point = pipeline_bind_point;
	cmd.uni.bind_descriptor_sets.layout = layout;
	cmd.uni.bind_descriptor_sets.first_set = first_set;
	cmd.uni.bind_descriptor_sets.descriptors_count = descriptors_count;
	cmd.uni.bind_descriptor_sets.descriptor_sets = descriptor_sets;
	cmd.uni.bind_descriptor_sets.offset_count = offset_count;
	cmd.uni.bind_descriptor_sets.offsets = offsets;

	return cmd;
}

LavCommand LAV_DrawIndexed(uint32_t index_count, uint32_t instance_count,	uint32_t first_index, int32_t vertex_offset, uint32_t first_instance){

	LavCommand cmd;

	cmd.type = LAV_CMD_DRAW_INDEXED;

	cmd.uni.draw_indexed.p = &vkCmdDrawIndexed;
	cmd.uni.draw_indexed.index_count = index_count;
	cmd.uni.draw_indexed.instance_count = instance_count;
	cmd.uni.draw_indexed.first_index = first_index;
	cmd.uni.draw_indexed.vertex_offset = vertex_offset;
	cmd.uni.draw_indexed.first_instance = first_instance;

	return cmd; 
}

LavCommand LAV_Draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance){

	LavCommand cmd;

	cmd.type =LAV_CMD_DRAW;

	cmd.uni.draw.p = &vkCmdDraw;
	cmd.uni.draw.vertex_count = vertex_count;
	cmd.uni.draw.instance_count = instance_count;
	cmd.uni.draw.first_vertex = first_vertex;
	cmd.uni.draw.first_instance = first_instance;

	return cmd;
}

void LAV_ExecuteCommands(VkCommandBuffer command_buffer, VkDescriptorSet *descriptor_sets, uint32_t count, LavCommand *cmd){

	for (int i = 0; i < count; ++i) {
		switch (cmd[i].type) {
			case LAV_CMD_BIND_PIPELINE:
				cmd[i].uni.bind_pipeline.p(command_buffer, cmd[i].uni.bind_pipeline.bind_point, cmd[i].uni.bind_pipeline.pipeline);
				break;
			case LAV_CMD_BIND_VERTEX_BUFFER:
				cmd[i].uni.bind_vertex_buffers.p(command_buffer, cmd[i].uni.bind_vertex_buffers.first_binding, cmd[i].uni.bind_vertex_buffers.bindings_count, cmd[i].uni.bind_vertex_buffers.buffers, cmd[i].uni.bind_vertex_buffers.offsets);
				break;
			case LAV_CMD_BIND_INDEX_BUFFER:
				cmd[i].uni.bind_index_buffers.p(command_buffer, cmd[i].uni.bind_index_buffers.buffer, cmd[i].uni.bind_index_buffers.offset, cmd[i].uni.bind_index_buffers.index_type);
				break;
			case LAV_CMD_BIND_DESCRIPTOR_SET:
				cmd[i].uni.bind_descriptor_sets.p(command_buffer, cmd[i].uni.bind_descriptor_sets.pipeline_bind_point, cmd[i].uni.bind_descriptor_sets.layout,cmd[i].uni.bind_descriptor_sets.first_set, cmd[i].uni.bind_descriptor_sets.descriptors_count, cmd[i].uni.bind_descriptor_sets.descriptor_sets, cmd[i].uni.bind_descriptor_sets.offset_count, cmd[i].uni.bind_descriptor_sets.offsets);
				break;
			case LAV_CMD_DRAW_INDEXED:
				cmd[i].uni.draw_indexed.p(command_buffer, cmd[i].uni.draw_indexed.index_count, cmd[i].uni.draw_indexed.instance_count, cmd[i].uni.draw_indexed.first_index, cmd[i].uni.draw_indexed.vertex_offset, cmd[i].uni.draw_indexed.first_instance);
				break;
			case LAV_CMD_DRAW:
				cmd[i].uni.draw.p(command_buffer, cmd[i].uni.draw.vertex_count, cmd[i].uni.draw.instance_count, cmd[i].uni.draw.first_vertex, cmd[i].uni.draw.first_instance);
				break;
			default:
				break;
		}
	}
}

void LAV_CreateCommandBuffers(LAV_Context *ctx, uint32_t count, LavCommand *cmd){

	VkCommandBufferAllocateInfo alloc_info = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
	alloc_info.commandPool = ctx->command_pool;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = ctx->swapchain_images_count;

	assert(vkAllocateCommandBuffers(ctx->device, &alloc_info, ctx->command_buffers) == VK_SUCCESS);


	VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

	VkClearValue depth_clear_value;
	depth_clear_value.depthStencil.depth = 1.0f;
	depth_clear_value.depthStencil.stencil = 0;

	VkClearValue clear_values[2] = {clear_color, depth_clear_value};

	for (int i = 0; i < ctx->swapchain_images_count; ++i) {

		VkCommandBufferBeginInfo begin_info = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
		begin_info.flags = 0; // Optional
		begin_info.pInheritanceInfo = NULL; // Optional

		assert(vkBeginCommandBuffer(ctx->command_buffers[i], &begin_info) == VK_SUCCESS);

		VkRenderPassBeginInfo render_pass_info = {.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
		render_pass_info.renderPass = ctx->render_pass;
		render_pass_info.framebuffer = ctx->swapchain_frame_buffers[i];
		render_pass_info.renderArea.offset.x = 0.0f;
		render_pass_info.renderArea.offset.y = 0.0f;
		render_pass_info.renderArea.extent.width = ctx->window_width;
		render_pass_info.renderArea.extent.height = ctx->window_height;
		render_pass_info.clearValueCount = NUM(clear_values);
		render_pass_info.pClearValues = clear_values;

		vkCmdBeginRenderPass(ctx->command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		LAV_ExecuteCommands(ctx->command_buffers[i], &ctx->descriptor_sets[i], count, cmd);

		vkCmdEndRenderPass(ctx->command_buffers[i]);
		assert(vkEndCommandBuffer(ctx->command_buffers[i]) == VK_SUCCESS);
	}
};

