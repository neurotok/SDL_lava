#include "SDL_lava_memory.h"

#define NUM(a) (sizeof(a)/sizeof(a[0]))

uint32_t VK_GetMemoryType(VK_Renderer *renderer, uint32_t type_filter, VkMemoryPropertyFlags properties){

	VkPhysicalDeviceMemoryProperties mem_properties;
	vkGetPhysicalDeviceMemoryProperties(renderer->physical_device, &mem_properties);

	for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
		if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) return i;
	}
	printf("Couldn't find memory type\n");
	return 0;
}

void VK_CreateBuffer(VK_Renderer *renderer, VkBuffer *buffer, VkDeviceMemory *data, VkDeviceSize size , VkBufferUsageFlags usage, VkMemoryPropertyFlags properties){

	VkBufferCreateInfo buffer_info = {.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
	buffer_info.size = size;
	buffer_info.usage = usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	buffer_info.flags = 0;

	assert(vkCreateBuffer(renderer->device, &buffer_info, NULL, buffer) == VK_SUCCESS);

	VkMemoryRequirements mem_requirements;
	vkGetBufferMemoryRequirements(renderer->device, *buffer, &mem_requirements);

	VkMemoryAllocateInfo alloc_info = {.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
	alloc_info.allocationSize = mem_requirements.size;
	alloc_info.memoryTypeIndex = VK_GetMemoryType(renderer,	mem_requirements.memoryTypeBits, properties);

	assert(vkAllocateMemory(renderer->device, &alloc_info, NULL, data) == VK_SUCCESS);

	vkBindBufferMemory(renderer->device, *buffer, *data, 0);
}

void VK_CopyBuffer(VK_Renderer *renderer, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) {
        VkCommandBuffer coppy_buffer = VK_BeginSingleTimeCommands(renderer);

        VkBufferCopy copy_region = {0};
        copy_region.size = size;

        vkCmdCopyBuffer(coppy_buffer, src_buffer, dst_buffer, 1, &copy_region);

        VK_EndSingleTimeCommands(renderer, &coppy_buffer);
    }

void VK_CreateVertexBuffer(VK_Renderer *renderer, mesh_t *mesh){

	VkDeviceSize buffer_size = mesh->vertices_size;

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_allocation;

	VK_CreateBuffer(renderer,
			&staging_buffer, &staging_buffer_allocation,
			buffer_size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* temp_data;// = malloc(buffer_size);
	vkMapMemory(renderer->device, staging_buffer_allocation, 0, buffer_size, 0, &temp_data);
	memcpy(temp_data, mesh->vertices, buffer_size);
	vkUnmapMemory(renderer->device, staging_buffer_allocation);
	//free(temp_data);

	VK_CreateBuffer(renderer,
			&renderer->vertex_buffer, &renderer->vertex_buffer_allocation,
			buffer_size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VK_CopyBuffer(renderer, staging_buffer, renderer->vertex_buffer, buffer_size);

	vkDestroyBuffer(renderer->device, staging_buffer, NULL);
	vkFreeMemory(renderer->device, staging_buffer_allocation, NULL);
}

void VK_CreateIndexBuffer(VK_Renderer *renderer, mesh_t *mesh){

	VkDeviceSize buffer_size = mesh->indices_size;

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_allocation;

	VK_CreateBuffer(renderer,
			&staging_buffer, &staging_buffer_allocation,
			buffer_size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* temp_data;
	vkMapMemory(renderer->device, staging_buffer_allocation, 0, buffer_size, 0, &temp_data);
	memcpy(temp_data, mesh->indices, buffer_size);
	vkUnmapMemory(renderer->device, staging_buffer_allocation);

	VK_CreateBuffer(renderer,
			&renderer->index_buffer, &renderer->index_buffer_allocation,
			buffer_size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VK_CopyBuffer(renderer, staging_buffer, renderer->index_buffer, buffer_size);

	vkDestroyBuffer(renderer->device, staging_buffer, NULL);
	vkFreeMemory(renderer->device, staging_buffer_allocation, NULL);
}

void VK_CreateUniformBuffer(VK_Renderer *renderer){

	VkDeviceSize buffer_size = sizeof(ubo_t);
	
	for (int i = 0; i < 2; ++i) {
			VK_CreateBuffer(renderer,
			&renderer->uniform_buffer[i], &renderer->uniform_buffer_allocation[i],
			buffer_size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}


