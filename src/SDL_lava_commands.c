#include "SDL_lava_commands.h"

#define NUM(a) (sizeof(a)/sizeof(a[0]))

//VK_BindVertexBuffer(VK_Command *command, uint32_t first_binding, uint32_t binding_count, const VkBuffer* buffer, const VkDeviceSize* offset){}

void VK_CreateCommandBuffers(VK_Context *ctx){

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

		vkCmdBindPipeline(ctx->command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, ctx->pip->graphics_pipeline);

		VkDeviceSize offsets[] = {0};

		vkCmdBindVertexBuffers(ctx->command_buffers[i], 0, 1, &ctx->vertex_buffer, offsets);
		vkCmdBindIndexBuffer(ctx->command_buffers[i], ctx->index_buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(ctx->command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, ctx->pip->pipeline_layout, 0, 1, &ctx->descriptor_sets[i], 0, NULL);
		vkCmdDrawIndexed(ctx->command_buffers[i], ctx->vertices, 1, 0, 0, 0);
		//vkCmdDraw(ctx->command_buffers[i], ctx->vertices, 1, 0, 0);

		vkCmdEndRenderPass(ctx->command_buffers[i]);
		assert(vkEndCommandBuffer(ctx->command_buffers[i]) == VK_SUCCESS);
	}
};

