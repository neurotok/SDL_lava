#include "SDL_lava_memory.h"

#define NUM(a) (sizeof(a)/sizeof(a[0]))

uint32_t LAV_GetMemoryType(LAV_Context *ctx, uint32_t type_filter, VkMemoryPropertyFlags properties){

	VkPhysicalDeviceMemoryProperties mem_properties;
	vkGetPhysicalDeviceMemoryProperties(ctx->physical_device, &mem_properties);

	for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
		if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) return i;
	}
	printf("Couldn't find memory type\n");
	return 0;
}

void LAV_CreateBuffer(LAV_Context *ctx, VkBuffer *buffer, VkDeviceMemory *data, VkDeviceSize size , VkBufferUsageFlags usage, VkMemoryPropertyFlags properties){

	VkBufferCreateInfo buffer_info = {.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
	buffer_info.size = size;
	buffer_info.usage = usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	buffer_info.flags = 0;

	assert(vkCreateBuffer(ctx->device, &buffer_info, NULL, buffer) == VK_SUCCESS);

	VkMemoryRequirements mem_requirements;
	vkGetBufferMemoryRequirements(ctx->device, *buffer, &mem_requirements);

	VkMemoryAllocateInfo alloc_info = {.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
	alloc_info.allocationSize = mem_requirements.size;
	alloc_info.memoryTypeIndex = LAV_GetMemoryType(ctx,	mem_requirements.memoryTypeBits, properties);

	assert(vkAllocateMemory(ctx->device, &alloc_info, NULL, data) == VK_SUCCESS);

	vkBindBufferMemory(ctx->device, *buffer, *data, 0);
}

void LAV_CopyBuffer(LAV_Context *ctx, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) {
        VkCommandBuffer coppy_buffer = LAV_BeginSingleTimeCommands(ctx);

        VkBufferCopy copy_region = {0};
        copy_region.size = size;

        vkCmdCopyBuffer(coppy_buffer, src_buffer, dst_buffer, 1, &copy_region);

        LAV_EndSingleTimeCommands(ctx, &coppy_buffer);
    }

void LAV_CreateVertexBuffer(LAV_Context *ctx, mesh_t *mesh){

	VkDeviceSize buffer_size = mesh->vertices_size;

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_allocation;

	LAV_CreateBuffer(ctx,
			&staging_buffer, &staging_buffer_allocation,
			buffer_size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* temp_data;// = malloc(buffer_size);
	vkMapMemory(ctx->device, staging_buffer_allocation, 0, buffer_size, 0, &temp_data);
	memcpy(temp_data, mesh->vertices, buffer_size);
	vkUnmapMemory(ctx->device, staging_buffer_allocation);
	//free(temp_data);

	LAV_CreateBuffer(ctx,
			&ctx->vertex_buffer, &ctx->vertex_buffer_allocation,
			buffer_size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	LAV_CopyBuffer(ctx, staging_buffer, ctx->vertex_buffer, buffer_size);

	vkDestroyBuffer(ctx->device, staging_buffer, NULL);
	vkFreeMemory(ctx->device, staging_buffer_allocation, NULL);
}

void LAV_CreateIndexBuffer(LAV_Context *ctx, mesh_t *mesh){

	VkDeviceSize buffer_size = mesh->indices_size;

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_allocation;

	LAV_CreateBuffer(ctx,
			&staging_buffer, &staging_buffer_allocation,
			buffer_size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* temp_data;
	vkMapMemory(ctx->device, staging_buffer_allocation, 0, buffer_size, 0, &temp_data);
	memcpy(temp_data, mesh->indices, buffer_size);
	vkUnmapMemory(ctx->device, staging_buffer_allocation);

	LAV_CreateBuffer(ctx,
			&ctx->index_buffer, &ctx->index_buffer_allocation,
			buffer_size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	LAV_CopyBuffer(ctx, staging_buffer, ctx->index_buffer, buffer_size);

	vkDestroyBuffer(ctx->device, staging_buffer, NULL);
	vkFreeMemory(ctx->device, staging_buffer_allocation, NULL);
}

void LAV_CreateUniformBuffer(LAV_Context *ctx, LAV_UniformBuffer *ubo){

	VkDeviceSize buffer_size = sizeof(ubo_t);
	
	for (int i = 0; i < 2; ++i) {
			LAV_CreateBuffer(ctx,
			&ubo->uniform_buffer[i], &ubo->uniform_buffer_allocation[i],
			buffer_size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}


