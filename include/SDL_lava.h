#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdalign.h>
#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "HandmadeMath.h"

typedef struct{
	uint32_t x;
	uint32_t y;
	uint32_t w;
	uint32_t h;
}LAV_Rect;

typedef enum{
	LAV_CTX_DEBUG = 1,
	LAV_CTX_DOUBLEBUF = 2,
	LAV_CTX_MIPMAPS = 4,
	LAV_CTX_MULTISAMPLING = 8,
}LAV_ContextMask;

/*
typedef enum{
	LAV_CULL_CLOCKWISE = 1,
	LAV_POINTS = 2,
	LAV_LINES = 4
}LAV_RasterizationMask;

typedef enum{
	LAV_DEPTH_TEST_DISABLE = 1,
	LAV_DEPTH_WRITE_DISABLE = 2
}LAV_DepthStencilMask;

typedef enum{
	LAV_BLEND_DISABLE = 1
}LAV_BlendMask;
*/

typedef enum{
	LAV_PIP_CULLING_CLOCKWISE = 1,
	LAV_PIP_MODE_POINTS = 2,
	LAV_PIP_MODE_LINES = 4,
	LAV_PIP_DEPTH_DISABLE = 8,
	LAV_PIP_BLEND_DISABLE = 16,
}LAV_PipelineMask;

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
	VkDescriptorSetLayout descriptor_layout;
	VkPipelineLayout pipeline_layout;
}LAV_PipelineLayout;

typedef struct{
	VkPipeline graphics_pipeline;
}LAV_Pipeline;


typedef struct{
	VkImage texture_image;
	VkDeviceMemory texture_image_allocation;
    VkImageView texture_image_view;
    VkSampler texture_sampler;
	VkDescriptorImageInfo image_info[2];
}LAV_Texture;

typedef struct{
	VkBuffer uniform_buffer[2];
	VkDeviceMemory uniform_buffer_allocation[2];
	VkDescriptorBufferInfo ubo_info[2];
}LAV_UniformBuffer;



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
	//LAV_Image
	uint32_t mips_level;

	//LAV_PipelineLayout *layout;
	//LAV_Pipeline *pip;
	//VkDescriptorSetLayout descriptor_layout;
	//VkPipelineLayout pipeline_layout;
	//VkPipeline graphics_pipeline;



	//LAV_Texture *tex;

	/*
    VkImage texture_image;
    VkDeviceMemory texture_image_allocation;
    VkImageView texture_image_view;
    VkSampler texture_sampler;
	*/

	VkBuffer vertex_buffer;
	VkDeviceMemory vertex_buffer_allocation;
	VkBuffer index_buffer;
	VkDeviceMemory index_buffer_allocation;

	uint32_t vertices;
	
	//LAV_UniformBuffer *ubo;
	//VkBuffer uniform_buffer[2];
	//VkDeviceMemory uniform_buffer_allocation[2];


	VkDescriptorPool descriptor_pool;
	VkDescriptorSet descriptor_sets[2];

	VkCommandBuffer command_buffers[2];

	VkSemaphore image_available_semaphore[2];
	VkSemaphore render_finished_semaphore[2];
	VkFence in_flight_fence[2];

}LAV_Context;


LAV_PipelineLayout* LAV_CreatePipelineLayout(LAV_Context *ctx, uint32_t bindings_count, VkDescriptorSetLayoutBinding bindings_description[], uint32_t push_constants_count, const VkPushConstantRange push_constants[]);

VkDescriptorSetLayoutBinding LAV_CreateBindingDescriptor(uint32_t binding, uint32_t count, VkDescriptorType type, VkShaderStageFlags flag);

LAV_Context* LAV_CreateContext(SDL_Window* window, const char *window_title, uint32_t instance_layers_count, const char *instance_layers[], uint32_t device_extensions_count, const char *device_extensions[], LAV_ContextMask context_mask);


VkVertexInputBindingDescription LAV_CreateVertexInputDescriptor(uint32_t binding, uint32_t stride, VkVertexInputRate input_rate);
VkVertexInputAttributeDescription LAV_CreateShaderDescriptor(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset);

LAV_Pipeline* LAV_CreatePipeline(LAV_Context *ctx, LAV_PipelineLayout *layout, uint32_t shaders_count, const char *shaders_sources[], uint32_t inputs_count,  VkVertexInputBindingDescription input_description[], uint32_t attributes_count , VkVertexInputAttributeDescription attribute_description[], LAV_PipelineMask pipeline_flags);


LAV_UniformBuffer* LAV_CreateUniformBuffer(LAV_Context *ctx, VkDeviceSize buffer_size);
LAV_Texture *LAV_CreateTexture(LAV_Context *ctx, const char *path);


void LAV_Rest(LAV_Context *ctx, LAV_PipelineLayout *layout, LAV_Pipeline *pip, LAV_UniformBuffer *ubo, LAV_Texture *tex);

void LAV_CreateImage(LAV_Context *ctx, VkImage *image, VkDeviceMemory *data,  uint32_t width, uint32_t height, uint32_t mip_levels, VkSampleCountFlagBits sample_count, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
VkImageView LAV_CreateImageView(LAV_Context *ctx, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t mip_levels, bool swizzle);
VkCommandBuffer LAV_BeginSingleTimeCommands(LAV_Context *ctx);
void LAV_EndSingleTimeCommands(LAV_Context *ctx, VkCommandBuffer *command_buffer);

void LAV_RecreateSwapchain(LAV_Context *ctx, SDL_Window *window, LAV_PipelineLayout *layout, LAV_Pipeline *pip, LAV_Texture *tex, LAV_UniformBuffer *ubo);
void LAV_DestroyContext(LAV_Context *ctx, LAV_PipelineLayout *layout, LAV_Pipeline *pip, LAV_Texture *tex, LAV_UniformBuffer *ubo);
