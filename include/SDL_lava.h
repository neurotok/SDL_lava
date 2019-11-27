#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdalign.h>
#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "HandmadeMath.h"

typedef enum{
	VK_RENDERER_DEBUG = 1,
	VK_RENDERER_DOUBLEBUF = 2,
	VK_RENDERER_MIPMAPS = 4,
	VK_RENDERER_MULTISAMPLING = 8,
}VK_RendererMask;

typedef enum{
	VK_PIPELINE_CULLING_CLOCKWISE = 1,
	VK_PIPELINE_DOUBLEBUF = 2,
	VK_PIPELINE_MIPMAPS = 4,
	VK_PIPELINE_MULTISAMPLING = 8,
}VK_PipelineMask;

typedef struct{
	float position[3];
	//float normals[3];
	float texcoord[2];
}vertex_t;

typedef struct{
	vertex_t *vertices;
	uint32_t *indices;
	size_t vertices_size;
	size_t indices_size;
}mesh_t;

typedef struct ubo_t {
	alignas(16) hmm_mat4 model;
	alignas(16) hmm_mat4 view;
	alignas(16) hmm_mat4 proj;
}ubo_t;

typedef struct{
	int window_width, window_height;
	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice physical_device;	
	VkPhysicalDeviceType physical_device_type;
	VkDevice device;
	uint32_t queue_family_index;
	VkColorSpaceKHR swapchain_color_space;
	VkFormat swapchain_image_format;
	uint32_t swapchain_images_count;
	VkSwapchainKHR swapchain;
	VkQueue device_queue;
	VkImage swapchain_images[2];
	VkImageView swapchain_images_views[2];
	VkFormat depth_image_format;
	VkSampleCountFlagBits sample_count;
	VkRenderPass render_pass;

	VkDescriptorSetLayout descriptor_layout;
	VkPipelineLayout pipeline_layout;

	VkPipeline graphics_pipeline;
	VkCommandPool command_pool;
	VkImage color_image;
	VkImage depth_image;
	VkImageView color_image_view;
	VkImageView depth_image_view;
	VkDeviceMemory color_image_allocation;
	VkDeviceMemory depth_image_allocation;
	VkFramebuffer swapchain_frame_buffers[2];
	bool gen_mips;
	float mips_max_level;
	//VK_Image
	uint32_t mips_level;

    VkImage texture_image;
    VkDeviceMemory texture_image_allocation;
    VkImageView texture_image_view;
    VkSampler texture_sampler;

	VkBuffer vertex_buffer;
	VkDeviceMemory vertex_buffer_allocation;
	VkBuffer index_buffer;
	VkDeviceMemory index_buffer_allocation;

	uint32_t vertices;

	VkBuffer uniform_buffer[2];
	VkDeviceMemory uniform_buffer_allocation[2];
	VkDescriptorPool descriptor_pool;
	VkDescriptorSet descriptor_sets[2];

	VkCommandBuffer command_buffers[2];

	VkSemaphore image_available_semaphore[2];
	VkSemaphore render_finished_semaphore[2];
	VkFence in_flight_fence[2];

}VK_Renderer;

VK_Renderer* VK_CreateRenderer(SDL_Window* window, const char *window_title, uint32_t instance_layers_count, const char *instance_layers[], uint32_t device_extensions_count, const char *device_extensions[], VK_RendererMask context_mask);
void VK_DestroyRenderer(VK_Renderer *renderer);

void VK_CreateImage(VK_Renderer *renderer, VkImage *image, VkDeviceMemory *data,  uint32_t width, uint32_t height, uint32_t mip_levels, VkSampleCountFlagBits sample_count, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
VkImageView VK_CreateImageView(VK_Renderer *renderer, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t mip_levels, bool swizzle);
VkCommandBuffer VK_BeginSingleTimeCommands(VK_Renderer *renderer);
void VK_EndSingleTimeCommands(VK_Renderer *renderer, VkCommandBuffer *command_buffer);

void VK_RecreateSwapchain(VK_Renderer *renderer, SDL_Window *window);

