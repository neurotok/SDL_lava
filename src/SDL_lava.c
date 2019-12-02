#include <stdbool.h>

#include <SDL2/SDL_vulkan.h>

#include "SDL_lava.h"
#include "SDL_lava_utils.h"
#include "SDL_lava_memory.h"
#include "SDL_lava_import.h"
#include "SDL_lava_commands.h"

#define NUM(a) (sizeof(a)/sizeof(a[0]))

void LAV_CreateSurface(LAV_Context *ctx, SDL_Window* window);
void LAV_CreateInstance(SDL_Window *window, LAV_Context *ctx, const char *window_title, uint32_t instance_layers_count, const char* instance_layers[], LAV_ContextMask context_mask);
void LAV_GetPhisicalDevice(LAV_Context *ctx);
void LAV_CreateDevice(LAV_Context *ctx, uint32_t instance_layers_count, const char *instance_layers[], uint32_t  device_extensions_count, const char *device_extensions[], LAV_ContextMask context_mask);
void LAV_GetSurfaceFormat(LAV_Context *ctx);
void LAV_CreateSwapchain(LAV_Context *ctx, SDL_Window *window);
void LAV_CreateSwapchainImageViews(LAV_Context *ctx);
void LAV_GetSampleCount(LAV_Context *ctx);
void LAV_GetDepthFormat(LAV_Context *ctx);
void LAV_CreateRenderPass(LAV_Context *ctx);
//void LAV_CreateDescriptionSetLayout(LAV_Context *ctx);
//void LAV_CreatePipelineLayout(LAV_Context *ctx);

VkDescriptorSetLayoutBinding LAV_CreateBindingDescriptor(uint32_t binding, uint32_t count, VkDescriptorType type, VkShaderStageFlags flag);

//VkDescriptorSetLayout LAV_CreateDescriptionSetLayout(LAV_Context *ctx, uint32_t count, VkDescriptorSetLayoutBinding bindings_description[]);

//VkPipelineLayout LAV_CreatePipelineLayout(LAV_Context *ctx, VkDescriptorSetLayout *descriptor_layout);

void LAV_CreateGraphicsPipeline(LAV_Context *ctx, LAV_PipelineLayout *layout);
void LAV_CreateCommandPool(LAV_Context *ctx);
void LAV_CreateColorResource(LAV_Context *ctx);
void LAV_CreateDepthResource(LAV_Context *ctx);
void LAV_CreateFramebuffers(LAV_Context *ctx);
void LAV_CreateDescriptionPool(LAV_Context *ctx);


void LAV_CreateDescriptorSets(LAV_Context *ctx, LAV_PipelineLayout *layout, LAV_Texture *tex, LAV_UniformBuffer *ubo);

void LAV_CreateSyncObjects(LAV_Context *ctx);

LAV_Context* LAV_CreateContext(SDL_Window *window, const char *window_title, uint32_t instance_layers_count, const char *instance_layers[], uint32_t device_extensions_count, const char *device_extensions[], LAV_ContextMask context_mask){

	LAV_Context *ctx = malloc(sizeof(LAV_Context));

	SDL_Vulkan_GetDrawableSize(window, &ctx->window_width, &ctx->window_height);

	ctx->instance = VK_NULL_HANDLE;
	ctx->physical_device = VK_NULL_HANDLE;
	ctx->surface = VK_NULL_HANDLE;
	ctx->device = VK_NULL_HANDLE;
	ctx->swapchain_images_count = 2;
	ctx->swapchain = VK_NULL_HANDLE;
	ctx->swapchain_image_format = VK_NULL_HANDLE;
	ctx->swapchain_color_space = VK_NULL_HANDLE;
	ctx->depth_image_format = VK_NULL_HANDLE;
	ctx->render_pass = VK_NULL_HANDLE;

	//ctx->layout->descriptor_layout = VK_NULL_HANDLE;
	//ctx->layout->pipeline_layout = VK_NULL_HANDLE;

	//ctx->pip->graphics_pipeline = VK_NULL_HANDLE;
	ctx->command_pool = VK_NULL_HANDLE;
	ctx->mips_max_level = 0.0;
	ctx->device_queue = VK_NULL_HANDLE;
	ctx->gen_mips = true;
	ctx->descriptor_pool = VK_NULL_HANDLE;

	//LAV_CreateContext
	LAV_CreateInstance(window, ctx, window_title, instance_layers_count, instance_layers, context_mask);
	LAV_CreateSurface(ctx, window);
	LAV_GetPhisicalDevice(ctx);
	LAV_CreateDevice(ctx, instance_layers_count, instance_layers, device_extensions_count, device_extensions, context_mask);
	LAV_GetSurfaceFormat(ctx);
	LAV_CreateSwapchain(ctx, window);
	LAV_CreateSwapchainImageViews(ctx);
	LAV_GetDepthFormat(ctx);
	LAV_GetSampleCount(ctx);
	LAV_CreateRenderPass(ctx);

	LAV_CreateCommandPool(ctx);
	LAV_CreateColorResource(ctx);
	LAV_CreateDepthResource(ctx);
	LAV_CreateFramebuffers(ctx);

	LAV_CreateDescriptionPool(ctx);
	LAV_CreateSyncObjects(ctx);
	/*
	const char *shader_sources[] = { "../assets/shaders/vert.spv",  "../assets/shaders/frag.spv"};

	VkVertexInputAttributeDescription attribute_description[] = {// = {position_attribute_description, textcoord_attribute_description};
		LAV_CreateShaderDescriptor(0,0, VK_FORMAT_R32G32B32_SFLOAT, 0),
		LAV_CreateShaderDescriptor(0,1, VK_FORMAT_R32G32_SFLOAT, 3 * sizeof(float))
	};

	ctx->pip->graphics_pipeline = LAV_CreateGraphicsPipeline(ctx,
			NUM(shader_sources),
			shader_sources,
			NUM(attribute_description),
			attribute_description,
			pipeline_creation_mask);
	*/

	
	/*

	LAV_CreateGraphicsPipeline(ctx);

	
	//LAV_CreateCommandPool(ctx);
	//LAV_CreateColorResource(ctx);
	//LAV_CreateDepthResource(ctx);
	//LAV_CreateFramebuffers(ctx);
	


	LAV_CreateTextureImage(ctx, "../assets/images/chalet.jpg");
	//LAV_CreateTextureImage(ctx, "../assets/models/DuckCM.png");
	LAV_CreateTextureImageView(ctx);
	LAV_CreateTextureSampler(ctx);
	VK_LoadModel(ctx, "../assets/models/chalet.obj");
	//VK_LoadModel(ctx, "../assets/models/Duck.obj`");
	LAV_CreateUniformBuffer(ctx);

	LAV_CreateDescriptionPool(ctx);
	LAV_CreateDescriptorSets(ctx);

	VkDeviceSize offsets[] = {0};

	command_t commands[] = {
		LAV_BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, ctx->pip->graphics_pipeline),	
		LAV_BindVertexBuffer(0,1, &ctx->vertex_buffer, offsets),
		LAV_BindIndexBuffer(ctx->index_buffer, 0, VK_INDEX_TYPE_UINT32),
		LAV_BindDescriptors(VK_PIPELINE_BIND_POINT_GRAPHICS, ctx->layout->pipeline_layout, 0, 1, ctx->descriptor_sets, 0, NULL),
		LAV_DrawIndexed(ctx->vertices, 1, 0, 0, 0)
		//LAV_Draw(ctx->vertices, 1, 0, 0)
	};

	LAV_CreateCommandBuffers(ctx, NUM(commands), commands);
	LAV_CreateSyncObjects(ctx);
	*/
	return ctx;
}

LAV_Texture *LAV_CreateTexture(LAV_Context *ctx, const char *path){

	LAV_Texture *tex = malloc(sizeof(LAV_Texture));

	LAV_CreateTextureImage(ctx, tex, path);
	LAV_CreateTextureImageView(ctx, tex);
	LAV_CreateTextureSampler(ctx, tex);

	for (int i = 0; i < 2; ++i) {
		//tex->image_info = {0};
		tex->image_info[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		tex->image_info[i].imageView = tex->texture_image_view;
		tex->image_info[i].sampler = tex->texture_sampler;
	}

	return tex;

}

void LAV_Rest(LAV_Context *ctx, LAV_PipelineLayout *layout, LAV_Pipeline *pip, LAV_UniformBuffer *ubo, LAV_Texture *tex){




	//LAV_CreateGraphicsPipeline(ctx, layout);

	
	//LAV_CreateCommandPool(ctx);
	//LAV_CreateColorResource(ctx);
	//LAV_CreateDepthResource(ctx);
	//LAV_CreateFramebuffers(ctx);
	
	/*
	ctx->tex = malloc(sizeof(LAV_Texture));
	LAV_CreateTextureImage(ctx, ctx->tex, "../assets/images/chalet.jpg");
	LAV_CreateTextureImageView(ctx, ctx->tex);
	LAV_CreateTextureSampler(ctx, ctx->tex);
	*/
	


	LAV_LoadModel(ctx, "../assets/models/chalet.obj");
	//VK_LoadModel(ctx, "../assets/models/Duck.obj`");
	/*
	ctx->ubo = malloc(sizeof(LAV_UniformBuffer));
	LAV_CreateUniformBuffer(ctx, ctx->ubo);
	*/

	//LAV_CreateDescriptionPool(ctx);
	/*
	LavDescriptors descriptors[] = {
		LAV_WriteUniformBuffer
		LAV_Write 
	}
	*/
	LAV_CreateDescriptorSets(ctx, layout, tex, ubo);

	/*

	VkDeviceSize offsets[] = {0};

	LavCommand commands[] = {
		LAV_BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pip->graphics_pipeline),	
		LAV_BindVertexBuffer(0,1, &ctx->vertex_buffer, offsets),
		LAV_BindIndexBuffer(ctx->index_buffer, 0, VK_INDEX_TYPE_UINT32),
		LAV_BindDescriptors(VK_PIPELINE_BIND_POINT_GRAPHICS, layout->pipeline_layout, 0, 1, ctx->descriptor_sets, 0, NULL),
		LAV_DrawIndexed(ctx->vertices, 1, 0, 0, 0)
		//LAV_Draw(ctx->vertices, 1, 0, 0)
	};

	LAV_CreateCommandBuffers(ctx, NUM(commands), commands);
	*/
	//LAV_CreateSyncObjects(ctx);
}

void LAV_CreateInstance(SDL_Window *window, LAV_Context *ctx, const char *window_title, uint32_t instance_layers_count, const char* instance_layers[], LAV_ContextMask context_mask){

	uint32_t instance_extensions_count;
	SDL_Vulkan_GetInstanceExtensions(window, &instance_extensions_count, NULL);
	const char* instance_extensions[instance_extensions_count];
	SDL_Vulkan_GetInstanceExtensions(window, &instance_extensions_count, instance_extensions);

	VkApplicationInfo app_info = {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO};
	app_info.pNext = NULL;
	app_info.pApplicationName = window_title;
	app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	app_info.pEngineName = "No name";
	app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	app_info.apiVersion = VK_MAKE_VERSION(1, 0, 21);

	VkInstanceCreateInfo instance_create_info = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
	instance_create_info.pNext = NULL;
	instance_create_info.flags = 0;
	instance_create_info.pApplicationInfo = &app_info;
	instance_create_info.enabledLayerCount = context_mask & LAV_CTX_DEBUG ? instance_layers_count : 0;
	instance_create_info.ppEnabledLayerNames = context_mask & LAV_CTX_DEBUG ? instance_layers : VK_NULL_HANDLE;
	instance_create_info.enabledExtensionCount = instance_extensions_count;
	instance_create_info.ppEnabledExtensionNames = instance_extensions,

	assert(vkCreateInstance(&instance_create_info, NULL, &ctx->instance) == VK_SUCCESS);
}

void LAV_CreateSurface(LAV_Context *ctx, SDL_Window* window){
	assert(SDL_Vulkan_CreateSurface(window, ctx->instance, &ctx->surface) == 1);
}

void LAV_GetPhisicalDevice(LAV_Context *ctx){

	uint32_t devices_count = 0;
	vkEnumeratePhysicalDevices(ctx->instance, &devices_count, NULL);
	VkPhysicalDevice devices[devices_count];
	vkEnumeratePhysicalDevices(ctx->instance, &devices_count, devices);

	for (uint32_t i = 0; i < devices_count; ++i)
	{
		uint32_t queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queue_family_count, NULL);
		VkQueueFamilyProperties queue_family_properties[queue_family_count];
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queue_family_count, queue_family_properties);

		for (uint32_t j = 0; j < queue_family_count; ++j) {

			VkBool32 supports_present = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(devices[i], j, ctx->surface, &supports_present);

			if (supports_present && (queue_family_properties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				ctx->queue_family_index = j;
				ctx->physical_device = devices[i];
				break;
			}
		}
		if (ctx->physical_device)
		{
			break;
		}
	}
}

void LAV_CreateDevice(LAV_Context *ctx, uint32_t instance_layers_count, const char *instance_layers[], uint32_t  device_extensions_count, const char *device_extensions[], LAV_ContextMask context_mask){

	VkDeviceQueueCreateInfo device_queue_create_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
	device_queue_create_info.pNext = NULL;
	device_queue_create_info.flags = 0;
	device_queue_create_info.queueFamilyIndex = ctx->queue_family_index;
	device_queue_create_info.queueCount	= 1;
	device_queue_create_info.pQueuePriorities = (const float []){1.0f};

	VkPhysicalDeviceFeatures supported_features;
	vkGetPhysicalDeviceFeatures(ctx->physical_device, &supported_features);
	//Anisotropy sampler support
	VkBool32 anisotropy_enable = supported_features.samplerAnisotropy;
	assert(anisotropy_enable);

	VkPhysicalDeviceFeatures device_features = {.samplerAnisotropy = VK_TRUE};

	VkDeviceCreateInfo device_create_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
	device_create_info.pNext = NULL;
	device_create_info.flags = 0;
	device_create_info.queueCreateInfoCount = 1;
	device_create_info.pQueueCreateInfos = &device_queue_create_info;
	device_create_info.enabledLayerCount = context_mask & LAV_CTX_DEBUG ? instance_layers_count : 0;
	device_create_info.ppEnabledLayerNames =  context_mask & LAV_CTX_DEBUG ? instance_layers : VK_NULL_HANDLE;
	device_create_info.enabledExtensionCount = device_extensions_count > 0 ? device_extensions_count : 0;
	device_create_info.ppEnabledExtensionNames =device_extensions_count > 0 ? device_extensions : VK_NULL_HANDLE;
	device_create_info.pEnabledFeatures = &device_features;

	assert(vkCreateDevice(ctx->physical_device, &device_create_info, NULL, &ctx->device) == VK_SUCCESS);

	vkGetDeviceQueue(ctx->device,ctx->queue_family_index, 0, &ctx->device_queue);
}

void LAV_GetDepthFormat(LAV_Context *ctx){

	VkFormat depth_formats[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
	VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
	VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

	for (unsigned long i = 0; i < NUM(depth_formats); i++ )
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(ctx->physical_device, depth_formats[i], &props);

		if ((tiling == VK_IMAGE_TILING_LINEAR) &&
				((props.linearTilingFeatures & features) == features))
		{
			ctx->depth_image_format = depth_formats[i];
			break;
		}
		else if ((tiling == VK_IMAGE_TILING_OPTIMAL) &&
				((props.optimalTilingFeatures & features) == features))
		{
			ctx->depth_image_format = depth_formats[i];
			break;
		}
	}

}

void LAV_GetSurfaceFormat(LAV_Context *ctx){

	uint32_t surface_formats_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(ctx->physical_device, ctx->surface, &surface_formats_count, NULL);
	assert(surface_formats_count);

	VkSurfaceFormatKHR surface_formats[surface_formats_count];
	vkGetPhysicalDeviceSurfaceFormatsKHR(ctx->physical_device, ctx->surface, &surface_formats_count, surface_formats);

	if ((surface_formats_count == 1) && (surface_formats[0].format == VK_FORMAT_UNDEFINED)){
		ctx->swapchain_image_format = VK_FORMAT_B8G8R8A8_UNORM;
		ctx->swapchain_color_space = surface_formats[0].colorSpace;
	}
	else {
		bool found_B8G8R8A8_UNORM = false;

		for (uint32_t i = 0; i < surface_formats_count; i++){
			if (surface_formats[i].format == VK_FORMAT_B8G8R8A8_UNORM){
				ctx->swapchain_image_format = surface_formats[i].format;
				ctx->swapchain_color_space = surface_formats[i].colorSpace;
				found_B8G8R8A8_UNORM = true;
				break;
			}
		}
		if (!found_B8G8R8A8_UNORM){
			ctx->swapchain_image_format = surface_formats[0].format;
			ctx->swapchain_color_space = surface_formats[0].colorSpace;
		}

	}
}

VkImageView LAV_CreateImageView(LAV_Context *ctx, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t mip_levels, bool swizzle){

	VkImageViewCreateInfo image_view_create_info = {.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
	image_view_create_info.image = image;
	image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	image_view_create_info.format = format;
	image_view_create_info.subresourceRange .aspectMask = aspect_flags;
	image_view_create_info.subresourceRange .baseMipLevel = 0;
	image_view_create_info.subresourceRange .levelCount = mip_levels;
	image_view_create_info.subresourceRange .baseArrayLayer = 0;
	image_view_create_info.subresourceRange .layerCount = 1;

	if(swizzle){

		image_view_create_info.components .r = VK_COMPONENT_SWIZZLE_B;
		image_view_create_info.components .g = VK_COMPONENT_SWIZZLE_G;
		image_view_create_info.components .b = VK_COMPONENT_SWIZZLE_R;
		image_view_create_info.components .a = VK_COMPONENT_SWIZZLE_IDENTITY;
	}

	VkImageView image_view;
	assert(vkCreateImageView(ctx->device, &image_view_create_info, NULL, &image_view) == VK_SUCCESS);

	return  image_view;
}

VkExtent2D LAV_GetSwapchainExtent(LAV_Context *ctx, SDL_Window *window, VkSurfaceCapabilitiesKHR *surface_capabilities){

	if (surface_capabilities->currentExtent.width != UINT32_MAX) {
		return surface_capabilities->currentExtent;
	} else {
		SDL_Vulkan_GetDrawableSize(window, &ctx->window_width, &ctx->window_height);

		VkExtent2D extent = {0};
		extent.width = clamp(ctx->window_width, surface_capabilities->minImageExtent.width, surface_capabilities->maxImageExtent.width);
		extent.height = clamp(ctx->window_height, surface_capabilities->minImageExtent.height, surface_capabilities->maxImageExtent.height); 

		return extent;
	}
}

void LAV_CreateSwapchain(LAV_Context *ctx, SDL_Window *window){

	VkSurfaceCapabilitiesKHR surface_capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx->physical_device, ctx->surface, &surface_capabilities);

	ctx->window_width = clamp(ctx->window_width, surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width);
	ctx->window_height = clamp(ctx->window_height, surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height); 

	VkSwapchainCreateInfoKHR swapchain_create_info = {.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
	swapchain_create_info.pNext = NULL;
	swapchain_create_info .flags = 0;
	swapchain_create_info .surface = ctx->surface;
	//In case of wayland it's 4
	//swapchain_create_info.minImageCount = ctx->surface_capabilities.minImageCount,
	swapchain_create_info.minImageCount = ctx->swapchain_images_count;
	swapchain_create_info.imageFormat = ctx->swapchain_image_format;
	swapchain_create_info.imageColorSpace = ctx->swapchain_color_space;
	swapchain_create_info.imageExtent.width = ctx->window_width;
	swapchain_create_info.imageExtent.height = ctx->window_height;

	swapchain_create_info.imageArrayLayers = 1;
	swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchain_create_info.queueFamilyIndexCount = ctx->queue_family_index;
	swapchain_create_info.pQueueFamilyIndices = NULL;
	swapchain_create_info.preTransform = surface_capabilities.currentTransform;
	swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain_create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	swapchain_create_info.clipped = VK_TRUE;
	swapchain_create_info.oldSwapchain = NULL;

	assert(vkCreateSwapchainKHR(ctx->device, &swapchain_create_info, NULL, &ctx->swapchain) == VK_SUCCESS);

	vkGetSwapchainImagesKHR(ctx->device, ctx->swapchain, &ctx->swapchain_images_count, NULL);
	assert(ctx->swapchain_images_count);
	vkGetSwapchainImagesKHR(ctx->device, ctx->swapchain, &ctx->swapchain_images_count, ctx->swapchain_images);
}

void LAV_CreateSwapchainImageViews(LAV_Context *ctx){

	for (uint32_t i = 0; i < ctx->swapchain_images_count; ++i) {

		ctx->swapchain_images_views[i] = LAV_CreateImageView(ctx,
				ctx->swapchain_images[i],
				ctx->swapchain_image_format,
				VK_IMAGE_ASPECT_COLOR_BIT,
				1,
				false);
	}

}

void LAV_GetSampleCount(LAV_Context *ctx){

	ctx->sample_count = VK_SAMPLE_COUNT_1_BIT;

	VkPhysicalDeviceProperties physical_device_properties;
	vkGetPhysicalDeviceProperties(ctx->physical_device, &physical_device_properties);

	VkSampleCountFlags color_sample_counts = physical_device_properties.limits.framebufferColorSampleCounts; 
	VkSampleCountFlags depth_sample_counts = physical_device_properties.limits.framebufferDepthSampleCounts; 
	VkSampleCountFlags counts = color_sample_counts < depth_sample_counts ? color_sample_counts : depth_sample_counts; 

	if (counts & VK_SAMPLE_COUNT_64_BIT) { ctx->sample_count = VK_SAMPLE_COUNT_64_BIT; }
	if (counts & VK_SAMPLE_COUNT_32_BIT) { ctx->sample_count = VK_SAMPLE_COUNT_32_BIT; }
	if (counts & VK_SAMPLE_COUNT_16_BIT) { ctx->sample_count = VK_SAMPLE_COUNT_16_BIT; }
	if (counts & VK_SAMPLE_COUNT_8_BIT) { ctx->sample_count = VK_SAMPLE_COUNT_8_BIT; }
	if (counts & VK_SAMPLE_COUNT_4_BIT) { ctx->sample_count = VK_SAMPLE_COUNT_4_BIT; }
	if (counts & VK_SAMPLE_COUNT_2_BIT) { ctx->sample_count = VK_SAMPLE_COUNT_2_BIT; }
}

void LAV_CreateRenderPass(LAV_Context *ctx){

	VkAttachmentDescription swachain_images = {0};
	swachain_images.format = ctx->swapchain_image_format;
	swachain_images.samples = ctx->sample_count;
	//swachain_images.samples = VK_SAMPLE_COUNT_1_BIT,
	swachain_images.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	swachain_images.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	swachain_images.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	swachain_images.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	swachain_images.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	swachain_images.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depth_resource = {0}; 
	depth_resource.format = ctx->depth_image_format;
	depth_resource.samples = ctx->sample_count;
	//depth_resource.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_resource.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_resource.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_resource.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depth_resource.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_resource.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_resource.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription color_resource = {0};
	color_resource.format = ctx->swapchain_image_format;
	color_resource.samples = VK_SAMPLE_COUNT_1_BIT;
	color_resource.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_resource.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_resource.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_resource.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_resource.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_resource.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; 

	VkAttachmentDescription render_pass_attachments[] = {swachain_images, depth_resource, color_resource};

	VkAttachmentReference swachain_images_reference = {0};
	swachain_images_reference.attachment = 0;
	swachain_images_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_resource_reference = {0};
	depth_resource_reference.attachment = 1;
	depth_resource_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference color_resource_reference = {0};
	color_resource_reference.attachment = 2;
	color_resource_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {0};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &swachain_images_reference;
	subpass.pDepthStencilAttachment = &depth_resource_reference;
	subpass.pResolveAttachments = &color_resource_reference;

	VkSubpassDependency dependency = {0};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo render_pass_info = {.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
	render_pass_info .attachmentCount = NUM(render_pass_attachments);
	render_pass_info .pAttachments = render_pass_attachments;
	render_pass_info .subpassCount = 1;
	render_pass_info .pSubpasses = &subpass;
	render_pass_info .dependencyCount = 1;
	render_pass_info .pDependencies = &dependency;

	assert(vkCreateRenderPass(ctx->device, &render_pass_info, NULL, &ctx->render_pass) == VK_SUCCESS);
}

VkDescriptorSetLayoutBinding LAV_CreateBindingDescriptor(uint32_t binding, uint32_t count, VkDescriptorType type, VkShaderStageFlags flag){

	VkDescriptorSetLayoutBinding bindings_description;
	bindings_description.binding = binding;
	bindings_description.descriptorCount = count;
	bindings_description.descriptorType = type;
	bindings_description.pImmutableSamplers = NULL;
	bindings_description.stageFlags = flag;

	return bindings_description;
}

VkPushConstantRange LAV_CreatePushCnstant(VkShaderStageFlags stage_flags, uint32_t offset, uint32_t size){

	VkPushConstantRange push_constant;
	push_constant.stageFlags = stage_flags;
	push_constant.offset = offset;
	push_constant.size = size;
		
	return push_constant;
}
/*
VkDescriptorSetLayout vk_createdescriptionsetlayout(LAV_Context *ctx, uint32_t count, VkDescriptorSetLayoutBinding bindings_description[]){

	VkDescriptorSetLayout bindings;

	VkDescriptorSetLayoutCreateInfo descriptor_layout_info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
	descriptor_layout_info.bindingCount = count;
	descriptor_layout_info.pBindings = bindings_description;

	assert(vkCreateDescriptorSetLayout(ctx->device, &descriptor_layout_info, NULL, &bindings) == VK_SUCCESS);

	return bindings;
}
*/
LAV_PipelineLayout* LAV_CreatePipelineLayout(LAV_Context *ctx, uint32_t bindings_count, VkDescriptorSetLayoutBinding bindings_description[], uint32_t push_constants_count, const VkPushConstantRange push_constants[]){

	LAV_PipelineLayout *layout = malloc(sizeof(LAV_PipelineLayout));

	//layout.descriptor_layout = LAV_CreateDescriptionSetLayout(ctx, bindings_count, bindings_description);

	VkDescriptorSetLayoutCreateInfo descriptor_layout_info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
	descriptor_layout_info.bindingCount = bindings_count;
	descriptor_layout_info.pBindings = bindings_description;

	assert(vkCreateDescriptorSetLayout(ctx->device, &descriptor_layout_info, NULL, &layout->descriptor_layout) == VK_SUCCESS);

	VkPipelineLayoutCreateInfo pipeline_layout_info = {.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
	pipeline_layout_info.setLayoutCount = 1; // Optional
	pipeline_layout_info.pSetLayouts = &layout->descriptor_layout; // Optional
	pipeline_layout_info.pushConstantRangeCount = push_constants_count; // Optional
	pipeline_layout_info.pPushConstantRanges = push_constants; // Optional

	assert(vkCreatePipelineLayout(ctx->device, &pipeline_layout_info, NULL, &layout->pipeline_layout) == VK_SUCCESS);

	return layout;
}

VkShaderModule LAV_CreateShaderModule(VkDevice device,const char *filename){
	FILE *file = fopen(filename,"r");

	if (!file) {
		printf("Can't open file: %s\n", filename);
		return 0;
	}

	size_t lenght = file_get_lenght(file);

	const uint32_t *shader_descripteion = (const uint32_t *)calloc(lenght + 1, 1);
	if (!shader_descripteion) {
		printf("Out of memory when reading file: %s\n",filename);
		fclose(file);
		file = NULL;
		return 0;;
	}

	fread((void *)shader_descripteion,1,lenght,file);
	fclose(file);

	file = NULL;

	VkShaderModuleCreateInfo shader_module_create_info = {.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
	shader_module_create_info.codeSize = lenght;
	shader_module_create_info.pCode	= shader_descripteion;

	VkShaderModule shader_module;
	assert(vkCreateShaderModule(device, &shader_module_create_info, NULL, &shader_module) == VK_SUCCESS); 
	free((void *)shader_descripteion);
	shader_descripteion = NULL;

	return shader_module;
}

VkVertexInputAttributeDescription LAV_CreateShaderDescriptor(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset){

	VkVertexInputAttributeDescription attribute = {0};
	attribute.binding = binding;
	attribute.location = location;
	attribute.format = format;
	attribute.offset = offset;

	return attribute;
}

VkVertexInputBindingDescription LAV_CreateVertexInputDescriptor(uint32_t binding, uint32_t stride, VkVertexInputRate input_rate){

	VkVertexInputBindingDescription vertex_input = {0};
	vertex_input.binding = binding;
	vertex_input.stride = stride;
	vertex_input.inputRate = input_rate;

	return vertex_input;
}

VkPipelineShaderStageCreateInfo LAV_CreateShaderStage(const char *path, VkShaderModule shader_module){


	VkPipelineShaderStageCreateInfo shader_stage = {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};

	shader_stage.module = shader_module;
	shader_stage.pName = "main";

	if (strstr(path, ".vert") || strstr(path, ".vs") || strstr(path, ".vertex")){
	
		shader_stage.stage = VK_SHADER_STAGE_VERTEX_BIT;

		return shader_stage;
	};

	if (strstr(path, ".frag") || strstr(path, ".fs") || strstr(path, ".fragment")){

		shader_stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

		return shader_stage;
	};

	printf("Error: could not detect shader type of the file %s, please use:\n .vert, .vertex, .vs file extensions for the vertex shader files\n	.frag, .fragment, .fs file extensions for the fragment shader files\n",	path);

	exit (0);

}


LAV_Pipeline* LAV_CreatePipeline(LAV_Context *ctx, LAV_PipelineLayout *layout, uint32_t shaders_count, const char *shaders_sources[], uint32_t inputs_count,  VkVertexInputBindingDescription input_description[], uint32_t attributes_count , VkVertexInputAttributeDescription attribute_description[], LAV_PipelineMask pipeline_flags){

	LAV_Pipeline *pipeline = malloc(sizeof(LAV_Pipeline));	


	VkShaderModule shader_modules[shaders_count];
	VkPipelineShaderStageCreateInfo shader_stages[shaders_count];

	for (int i = 0; i < shaders_count; ++i) {
		shader_modules[i] = LAV_CreateShaderModule(ctx->device, shaders_sources[i]);
		shader_stages[i] = LAV_CreateShaderStage(shaders_sources[i], shader_modules[i]); 
	}

	VkPipelineVertexInputStateCreateInfo vertex_input_info = {.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
	vertex_input_info.vertexBindingDescriptionCount = inputs_count;
	vertex_input_info.pVertexBindingDescriptions = input_description; // &bindings_description;
	vertex_input_info.vertexAttributeDescriptionCount = attributes_count; //NUM(attribute_description);
	vertex_input_info.pVertexAttributeDescriptions = attribute_description;

	VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
	input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_info.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {0};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) ctx->window_width;
	viewport.height = (float) ctx->window_height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissors = {0};
	scissors.offset.x = 0;
	scissors.offset.y = 0;
	scissors.extent.width = ctx->window_width;
	scissors.extent.height = ctx->window_height;

	VkPipelineViewportStateCreateInfo viewport_info = {.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
	viewport_info.viewportCount = 1;
	viewport_info.pViewports = &viewport;
	viewport_info.scissorCount = 1;
	viewport_info.pScissors = &scissors;

	VkPipelineRasterizationStateCreateInfo rasterization_info = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
	rasterization_info.depthClampEnable = VK_FALSE;
	rasterization_info.rasterizerDiscardEnable = VK_FALSE;
	rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization_info.lineWidth = 1.0f;
	rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterization_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterization_info.depthBiasEnable = VK_FALSE;
	rasterization_info.depthBiasConstantFactor = 0.0f;
	rasterization_info.depthBiasClamp = 0.0f;
	rasterization_info.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisample_info = {.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
	multisample_info.sampleShadingEnable = VK_FALSE;
	//multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisample_info.rasterizationSamples = ctx->sample_count;
	multisample_info.minSampleShading = 1.0f;
	multisample_info.pSampleMask = NULL;
	multisample_info.alphaToCoverageEnable = VK_FALSE;
	multisample_info.alphaToOneEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
	depth_stencil_info.depthTestEnable = VK_TRUE;
	depth_stencil_info.depthWriteEnable = VK_TRUE;
	depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
	depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
	//depth_stencil_info.minDepthBounds = 0.0f; // Optional
	//depth_stencil_info.maxDepthBounds = 1.0f; // OptionaldepthStencil.stencilTestEnable = VK_FALSE;
	//depth_stencil_info.stencilTestEnable = VK_FALSE;
	//depth_stencil_info.front = {0}; // Optional
	//depth_stencil_info.back = {0}; // Optional

	VkPipelineColorBlendAttachmentState color_blend_attachment_info = {0};
	color_blend_attachment_info.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment_info.blendEnable = VK_FALSE;
	color_blend_attachment_info.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment_info.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment_info.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment_info.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment_info.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment_info.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo color_blend_info = {.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
	color_blend_info.logicOpEnable = VK_FALSE;
	color_blend_info.logicOp = VK_LOGIC_OP_COPY; // Optional
	color_blend_info.attachmentCount = 1;
	color_blend_info.pAttachments = &color_blend_attachment_info;
	color_blend_info.blendConstants[0] = 0.0f; // Optional
	color_blend_info.blendConstants[1] = 0.0f; // Optional
	color_blend_info.blendConstants[2] = 0.0f; // Optional
	color_blend_info.blendConstants[3] = 0.0f; // Optional

	VkGraphicsPipelineCreateInfo pipeline_info = {.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
	pipeline_info.stageCount = shaders_count;
	pipeline_info.pStages = shader_stages;
	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly_info;
	pipeline_info.pViewportState = &viewport_info;
	pipeline_info.pRasterizationState = &rasterization_info;
	pipeline_info.pMultisampleState = &multisample_info;		//Optional depth and stencil	
	pipeline_info.pDepthStencilState = &depth_stencil_info;
	pipeline_info.pColorBlendState = &color_blend_info;
	pipeline_info.pDynamicState = NULL; // Optional
	pipeline_info.layout = layout->pipeline_layout;
	pipeline_info.renderPass = ctx->render_pass;
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipeline_info.basePipelineIndex = -1; // Optional

	assert(vkCreateGraphicsPipelines(ctx->device, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &pipeline->graphics_pipeline) == VK_SUCCESS);

	for (int i = 0; i < shaders_count; ++i) {
		vkDestroyShaderModule(ctx->device, shader_modules[i], NULL);
	}

	return pipeline;
}

void LAV_CreateCommandPool(LAV_Context *ctx){

	VkCommandPoolCreateInfo pool_info = {.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
	pool_info.queueFamilyIndex = ctx->queue_family_index; //assuming one graphics queue family
	pool_info.flags = 0; // Optional

	assert(vkCreateCommandPool(ctx->device, &pool_info, NULL, &ctx->command_pool) == VK_SUCCESS);
}


void LAV_CreateImage(LAV_Context *ctx, VkImage *image, VkDeviceMemory *data,  uint32_t width, uint32_t height, uint32_t mip_levels, VkSampleCountFlagBits sample_count,  VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties){

	VkImageCreateInfo image_info = {.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = width;
	image_info.extent.height = height;
	image_info.extent.depth = 1;
	image_info.mipLevels = mip_levels;
	image_info.arrayLayers = 1;
	image_info.format = format;
	image_info.tiling = tiling;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = usage;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.samples = sample_count;
	image_info.flags = 0;

	assert(vkCreateImage(ctx->device, &image_info, NULL, image) == VK_SUCCESS);

	VkMemoryRequirements image_mem_requirements;
	vkGetImageMemoryRequirements(ctx->device, *image, &image_mem_requirements);

	VkMemoryAllocateInfo alloc_info = {.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
	alloc_info.allocationSize = image_mem_requirements.size;
	alloc_info.memoryTypeIndex = LAV_GetMemoryType(ctx, image_mem_requirements.memoryTypeBits, properties);

	assert(vkAllocateMemory(ctx->device, &alloc_info, NULL, data) == VK_SUCCESS);

	vkBindImageMemory(ctx->device, *image, *data, 0);
}

void LAV_CreateColorResource(LAV_Context *ctx){

	LAV_CreateImage(ctx,
			&ctx->color_image, &ctx->color_image_allocation,
			ctx->window_width, ctx->window_height,
			1, ctx->sample_count,
			ctx->swapchain_image_format,  
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	ctx->color_image_view = LAV_CreateImageView(ctx,
			ctx->color_image, ctx->swapchain_image_format,
			VK_IMAGE_ASPECT_COLOR_BIT, 1,
			false);
}

void LAV_CreateDepthResource(LAV_Context *ctx){

	LAV_CreateImage(ctx,
			&ctx->depth_image, &ctx->depth_image_allocation,
			ctx->window_width, ctx->window_height,
			1, ctx->sample_count,
			ctx->depth_image_format, 
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	ctx->depth_image_view = LAV_CreateImageView(ctx,
			ctx->depth_image, ctx->depth_image_format,
			VK_IMAGE_ASPECT_DEPTH_BIT, 1,
			false);
}

void LAV_CreateFramebuffers(LAV_Context *ctx){

	for (uint32_t i = 0; i < ctx->swapchain_images_count; ++i) {

		VkImageView framebuffer_attachments[] = {
			ctx->color_image_view,
			ctx->depth_image_view,
			ctx->swapchain_images_views[i]
		};

		VkFramebufferCreateInfo framebuffer_info = {.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
		framebuffer_info.renderPass = ctx->render_pass;
		framebuffer_info.attachmentCount = NUM(framebuffer_attachments);
		framebuffer_info.pAttachments = framebuffer_attachments;
		framebuffer_info.width = ctx->window_width;
		framebuffer_info.height = ctx->window_height;
		framebuffer_info.layers = 1;

		assert(vkCreateFramebuffer(ctx->device, &framebuffer_info, NULL, &ctx->swapchain_frame_buffers[i]) == VK_SUCCESS);
	}
}

VkCommandBuffer LAV_BeginSingleTimeCommands(LAV_Context *ctx) {

	VkCommandBufferAllocateInfo alloc_info = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = ctx->command_pool;
	alloc_info.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
	vkAllocateCommandBuffers(ctx->device, &alloc_info, &command_buffer);

	VkCommandBufferBeginInfo begin_info = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(command_buffer, &begin_info);

	return command_buffer;
}

void LAV_EndSingleTimeCommands(LAV_Context *ctx, VkCommandBuffer *command_buffer) {

	vkEndCommandBuffer(*command_buffer);

	VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO};
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = command_buffer;

	vkQueueSubmit(ctx->device_queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(ctx->device_queue);

	vkFreeCommandBuffers(ctx->device, ctx->command_pool, 1, command_buffer);
}


LAV_UniformBuffer* LAV_CreateUniformBuffer(LAV_Context *ctx, VkDeviceSize buffer_size){

	//VkDeviceSize buffer_size = sizeof(ubo_t);
	
	LAV_UniformBuffer *ubo = malloc(sizeof(LAV_UniformBuffer));

	for (int i = 0; i < 2; ++i) {
			LAV_CreateBuffer(ctx,
			&ubo->uniform_buffer[i], &ubo->uniform_buffer_allocation[i],
			buffer_size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			//ubo->buffer_info[i] = {0};
			ubo->ubo_info[i].buffer = ubo->uniform_buffer[i];
			ubo->ubo_info[i].range = buffer_size;
			ubo->ubo_info[i].offset = 0;
	}

	return ubo;
}


void LAV_CreateDescriptionPool(LAV_Context *ctx){

	VkDescriptorPoolSize ubo_descriptor = {0};
	ubo_descriptor.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubo_descriptor.descriptorCount = ctx->swapchain_images_count;

	VkDescriptorPoolSize sampler_descriptor = {0};
	sampler_descriptor.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler_descriptor.descriptorCount = ctx->swapchain_images_count;

	VkDescriptorPoolSize descriptor_pool_size[] = {ubo_descriptor, sampler_descriptor};

	VkDescriptorPoolCreateInfo descriptor_pool_info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
	descriptor_pool_info.poolSizeCount = NUM(descriptor_pool_size);
	descriptor_pool_info.pPoolSizes = descriptor_pool_size;
	descriptor_pool_info.maxSets = ctx->swapchain_images_count;

	assert(vkCreateDescriptorPool(ctx->device, &descriptor_pool_info, NULL, &ctx->descriptor_pool) == VK_SUCCESS);
}


void LAV_CreateDescriptorSets(LAV_Context *ctx, LAV_PipelineLayout *layout, LAV_Texture *tex, LAV_UniformBuffer *ubo){

	VkDescriptorSetLayout descriptor_sets_layout[ctx->swapchain_images_count];

	for (uint32_t i = 0; i < ctx->swapchain_images_count; ++i) {
		descriptor_sets_layout[i] = layout->descriptor_layout;
	}

	VkDescriptorSetAllocateInfo descriptor_alloc_info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
	descriptor_alloc_info.descriptorPool = ctx->descriptor_pool,
	descriptor_alloc_info.descriptorSetCount = ctx->swapchain_images_count,
	descriptor_alloc_info.pSetLayouts = descriptor_sets_layout;

	assert(vkAllocateDescriptorSets(ctx->device, &descriptor_alloc_info, ctx->descriptor_sets) == VK_SUCCESS);

	for (int i = 0; i < ctx->swapchain_images_count; ++i) {
		/*
		VkDescriptorBufferInfo ubo_info = {0};
		ubo_info.buffer = ubo->uniform_buffer[i];
		ubo_info.offset = 0;
		ubo_info.range = sizeof(ubo_t);
		*/
		VkWriteDescriptorSet ubo_write = {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
		ubo_write.dstSet = ctx->descriptor_sets[i];
		ubo_write.dstBinding = 0;
		ubo_write.dstArrayElement = 0;
		ubo_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		ubo_write.descriptorCount = 1;
		ubo_write.pBufferInfo = ubo->ubo_info;
		ubo_write.pImageInfo = NULL; // Optional
		ubo_write.pTexelBufferView = NULL; // Optional
		/*
		VkDescriptorImageInfo image_info = {0};
		image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_info.imageView = ctx->tex->texture_image_view;
		image_info.sampler = tex->texture_sampler;
		*/
		VkWriteDescriptorSet image_write = {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
		image_write.dstSet = ctx->descriptor_sets[i];
		image_write.dstBinding = 1;
		image_write.dstArrayElement = 0;
		image_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		image_write.descriptorCount = 1;
		image_write.pImageInfo = tex->image_info;

		VkWriteDescriptorSet descriptor_write[] = {ubo_write, image_write};

		vkUpdateDescriptorSets(ctx->device, NUM(descriptor_write), descriptor_write, 0, NULL);
	}
}

void LAV_CreateSyncObjects(LAV_Context *ctx){

	VkSemaphoreCreateInfo semaphore_info = {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

	VkFenceCreateInfo fence_info = {.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (int i = 0; i < 2; ++i) {
		assert(vkCreateSemaphore(ctx->device, &semaphore_info, NULL, &ctx->image_available_semaphore[i]) == VK_SUCCESS);
		assert(vkCreateSemaphore(ctx->device, &semaphore_info, NULL, &ctx->render_finished_semaphore[i]) == VK_SUCCESS);
		assert(vkCreateFence(ctx->device, &fence_info, NULL, &ctx->in_flight_fence[i]) == VK_SUCCESS);
	}
}

void LAV_DestroySwapchain(LAV_Context *ctx, LAV_PipelineLayout *layout, LAV_Pipeline *pip, LAV_UniformBuffer *ubo, LAV_CommandBuffer *cbo){

	vkDestroyImageView(ctx->device, ctx->depth_image_view, NULL);
	vkDestroyImage(ctx->device, ctx->depth_image, NULL);
	vkFreeMemory(ctx->device, ctx->depth_image_allocation, NULL);

	vkDestroyImageView(ctx->device, ctx->color_image_view, NULL);
	vkDestroyImage(ctx->device, ctx->color_image, NULL);
	vkFreeMemory(ctx->device, ctx->color_image_allocation, NULL);

	for (int i = 0; i < ctx->swapchain_images_count; i++) {
		vkDestroyFramebuffer(ctx->device, ctx->swapchain_frame_buffers[i], NULL);
	}

	vkFreeCommandBuffers(ctx->device, ctx->command_pool,  ctx->swapchain_images_count, cbo->command_buffers); 

	vkDestroyPipeline(ctx->device, pip->graphics_pipeline, NULL);
	vkDestroyPipelineLayout(ctx->device, layout->pipeline_layout, NULL);
	vkDestroyRenderPass(ctx->device, ctx->render_pass, NULL);

	for (int i = 0; i < ctx->swapchain_images_count; i++) {
		vkDestroyImageView(ctx->device, ctx->swapchain_images_views[i], NULL);
	}

	vkDestroySwapchainKHR(ctx->device, ctx->swapchain, NULL);

	for (int i = 0; i < ctx->swapchain_images_count; i++) {
		vkDestroyBuffer(ctx->device, ubo->uniform_buffer[i], NULL);
		vkFreeMemory(ctx->device, ubo->uniform_buffer_allocation[i], NULL);
	}

	vkDestroyDescriptorPool(ctx->device, ctx->descriptor_pool, NULL);
}
void LAV_DestroyContext(LAV_Context *ctx, LAV_PipelineLayout *layout, LAV_Pipeline *pip, LAV_Texture *tex, LAV_UniformBuffer *ubo, LAV_CommandBuffer *cbo){

	vkDeviceWaitIdle(ctx->device);

	LAV_DestroySwapchain(ctx, layout, pip,  ubo, cbo);

	//Texture
	vkDestroySampler(ctx->device, tex->texture_sampler, NULL);
	vkDestroyImageView(ctx->device, tex->texture_image_view, NULL);
	vkDestroyImage(ctx->device, tex->texture_image, NULL);
	vkFreeMemory(ctx->device, tex->texture_image_allocation, NULL);

	vkDestroyDescriptorSetLayout(ctx->device, layout->descriptor_layout, NULL);

	vkDestroyBuffer(ctx->device, ctx->index_buffer, NULL);
	vkFreeMemory(ctx->device, ctx->index_buffer_allocation, NULL);

	vkDestroyBuffer(ctx->device, ctx->vertex_buffer, NULL);
	vkFreeMemory(ctx->device, ctx->vertex_buffer_allocation, NULL);

	for (size_t i = 0; i < ctx->swapchain_images_count; i++) {
		vkDestroySemaphore(ctx->device, ctx->render_finished_semaphore[i], NULL);
		vkDestroySemaphore(ctx->device, ctx->image_available_semaphore[i], NULL);
		vkDestroyFence(ctx->device, ctx->in_flight_fence[i], NULL);
	}

	vkDestroyCommandPool(ctx->device, ctx->command_pool, NULL);

	vkDestroyDevice(ctx->device, NULL);
	vkDestroySurfaceKHR(ctx->instance, ctx->surface, NULL);
	vkDestroyInstance(ctx->instance, NULL);

	free(ctx);
}

void LAV_RecreateSwapchain(LAV_Context *ctx, SDL_Window *window, LAV_PipelineLayout *layout, LAV_Pipeline *pip, LAV_Texture *tex, LAV_UniformBuffer *ubo, LAV_CommandBuffer *cbo){

	vkDeviceWaitIdle(ctx->device);
	
	LAV_DestroySwapchain(ctx, layout, pip,  ubo, cbo);

	LAV_CreateSwapchain(ctx, window);
	LAV_CreateSwapchainImageViews(ctx);
	LAV_CreateRenderPass(ctx);
	//LAV_CreatePipeline(ctx, layout);
	LAV_CreateColorResource(ctx);
	LAV_CreateDepthResource(ctx);
	LAV_CreateFramebuffers(ctx);
	//LAV_CreateUniformBuffer(ctx, ubo);
	LAV_CreateDescriptionPool(ctx);
	LAV_CreateDescriptorSets(ctx, layout, tex, ubo);
	//LAV_CreateCommandBuffers(ctx);
}


