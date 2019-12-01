#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "SDL_lava.h"
#include "SDL_lava_memory.h"
#include "SDL_lava_import.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

void LAV_TransitionImageLayout(LAV_Context *ctx, VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout, uint32_t mip_levels);
void LAV_CopyBufferToImage(LAV_Context *ctx, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
void LAV_GenerateMipmaps(LAV_Context *ctx, VkImage image, VkFormat format, uint32_t width, uint32_t height, uint32_t mip_levels);

void LAV_CreateTextureImage(LAV_Context *ctx, LAV_Texture *tex, const char *file){

	int texture_width, texture_height, texture_channels;
	stbi_uc *pixels = stbi_load(file, &texture_width, &texture_height, &texture_channels, STBI_rgb_alpha);
	assert(pixels);

	uint32_t mip_levels = floor(log2(texture_width > texture_height ? texture_width : texture_height)) + 1;

	if (ctx->mips_max_level < mip_levels) {
		ctx->mips_max_level = (float)mip_levels;
	}

	VkDeviceSize texture_size = texture_width * texture_height * 4; //4 bytex per pixel

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_allocation;

	LAV_CreateBuffer(ctx,
			&staging_buffer, &staging_buffer_allocation,
			texture_size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* temp_data;
	vkMapMemory(ctx->device, staging_buffer_allocation, 0, texture_size, 0, &temp_data);
	memcpy(temp_data, pixels, texture_size);
	vkUnmapMemory(ctx->device, staging_buffer_allocation);

	stbi_image_free(pixels);

	LAV_CreateImage(ctx,
			&tex->texture_image, &tex->texture_image_allocation,
			texture_width, texture_height,
			mip_levels, VK_SAMPLE_COUNT_1_BIT, 
			ctx->swapchain_image_format, 
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	LAV_TransitionImageLayout(ctx,
			tex->texture_image,
			ctx->swapchain_image_format,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			mip_levels);

	LAV_CopyBufferToImage(ctx, staging_buffer, tex->texture_image, texture_width, texture_height);
	//TODO
	/*
	LAV_TransitionImageLayout(ctx,
			ctx->texture_image,
			ctx->swapchain_image_format,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			mip_levels);
	*/

	vkDestroyBuffer(ctx->device, staging_buffer, NULL);
	vkFreeMemory(ctx->device, staging_buffer_allocation, NULL);

	LAV_GenerateMipmaps(ctx,
			tex->texture_image,
			ctx->swapchain_image_format,
			texture_width, texture_height,
			mip_levels);
}

void LAV_TransitionImageLayout(LAV_Context *ctx, VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout, uint32_t mip_levels){

	VkCommandBuffer texture_upload = LAV_BeginSingleTimeCommands(ctx);

	VkImageMemoryBarrier image_mem_barrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
	image_mem_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_mem_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_mem_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_mem_barrier.subresourceRange.baseMipLevel = 0;
	image_mem_barrier.subresourceRange.levelCount = mip_levels;
	image_mem_barrier.subresourceRange.baseArrayLayer = 0;
	image_mem_barrier.subresourceRange.layerCount = 1;
	image_mem_barrier.oldLayout = old_layout;
	image_mem_barrier.newLayout = new_layout;
	image_mem_barrier.image = image;

	VkPipelineStageFlags source_stage;
	VkPipelineStageFlags destination_stage;

	if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		image_mem_barrier.srcAccessMask = 0;
		image_mem_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		image_mem_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		image_mem_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else {
		printf("Unsupported layout transition!\n");
	}

	vkCmdPipelineBarrier(texture_upload,
			source_stage,
			destination_stage,
			0,
			0, NULL,
			0, NULL,
			1, &image_mem_barrier);

	LAV_EndSingleTimeCommands(ctx, &texture_upload);
}
void LAV_CopyBufferToImage(LAV_Context *ctx, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height){

	VkCommandBuffer coppy_image = LAV_BeginSingleTimeCommands(ctx);

	VkBufferImageCopy region = {0};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageExtent.width = width;
	region.imageExtent.height = height;
	region.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(coppy_image, buffer, image,  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	LAV_EndSingleTimeCommands(ctx, &coppy_image);
}

void LAV_GenerateMipmaps(LAV_Context *ctx, VkImage image, VkFormat format, uint32_t width, uint32_t height, uint32_t mip_levels){

	VkCommandBuffer mips_generation = LAV_BeginSingleTimeCommands(ctx);

	VkImageMemoryBarrier mips_mem_barrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
	mips_mem_barrier.image = image;
	mips_mem_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	mips_mem_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	mips_mem_barrier.subresourceRange .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	mips_mem_barrier.subresourceRange .baseArrayLayer = 0;
	mips_mem_barrier.subresourceRange .levelCount = 1;
	mips_mem_barrier.subresourceRange .layerCount = 1;

	int32_t mip_width = width;
	int32_t mip_height = height;

	for (uint32_t i = 1; i < mip_levels; i++){

		mips_mem_barrier.subresourceRange.baseMipLevel = i - 1;
		mips_mem_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		mips_mem_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		mips_mem_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		mips_mem_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(mips_generation,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0, NULL,
				0, NULL,
				1, &mips_mem_barrier);

		VkImageBlit blit = {{0}};
		blit.srcOffsets[0].x = 0;
		blit.srcOffsets[0].y = 0;
		blit.srcOffsets[0].x = 0;
		blit.srcOffsets[1].x = mip_width;
		blit.srcOffsets[1].y = mip_height;
		blit.srcOffsets[1].z = 1;
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0].x = 0;
		blit.dstOffsets[0].y = 0;
		blit.dstOffsets[0].z = 0;
		blit.dstOffsets[1].x = mip_width > 1 ? mip_width / 2 : 1;
		blit.dstOffsets[1].y = mip_height > 1 ? mip_height / 2 : 1;
		blit.dstOffsets[1].z = 1;
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(mips_generation,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

		mips_mem_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		mips_mem_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		mips_mem_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		mips_mem_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(mips_generation,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				0,
				0, NULL,
				0, NULL,
				1, &mips_mem_barrier);

		if (mip_width > 1) mip_width /= 2;
		if (mip_height > 1) mip_height /= 2;
	}

	mips_mem_barrier.subresourceRange .baseMipLevel = mip_levels - 1;
	mips_mem_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	mips_mem_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	mips_mem_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	mips_mem_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(
			mips_generation,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0, NULL,
			0, NULL,
			1, &mips_mem_barrier);

	LAV_EndSingleTimeCommands(ctx, &mips_generation);
}

void LAV_CreateTextureImageView(LAV_Context *ctx, LAV_Texture *tex){
//void LAV_CreateTextureImageView(LAV_Context *ctx){
	tex->texture_image_view = LAV_CreateImageView(ctx,
			tex->texture_image, ctx->swapchain_image_format,
			VK_IMAGE_ASPECT_COLOR_BIT, 1,
			true);
}

void LAV_CreateTextureSampler(LAV_Context *ctx, LAV_Texture *tex){

	VkSamplerCreateInfo sampler_info = {.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
	sampler_info.magFilter = VK_FILTER_LINEAR;
	sampler_info.minFilter = VK_FILTER_LINEAR;
	sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.anisotropyEnable = VK_TRUE;
	sampler_info.maxAnisotropy = 16;
	sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable = VK_FALSE;
	sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
	sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_info.minLod = 0.0f;
	sampler_info.maxLod = ctx->gen_mips ? ctx->mips_max_level : 0.0f;
	sampler_info.mipLodBias = 0.0f;

	assert(vkCreateSampler(ctx->device, &sampler_info, NULL, &tex->texture_sampler) == VK_SUCCESS);
}

void LAV_ParseOBJ(const char *path, mesh_t *mesh){

	fastObjMesh* obj = fast_obj_read(path);
	if (!obj){
		printf("Error loading %s: file not found\n", path);
	}

	size_t total_indices = 0;

	for (unsigned int i = 0; i < obj->face_count; ++i) total_indices += 3 * (obj->face_vertices[i] - 2);
	

	mesh->vertices_size = sizeof(vertex_t) * total_indices;
	mesh->indices_size= sizeof(uint32_t) * total_indices;

	mesh->vertices = malloc(mesh->vertices_size);
	mesh->indices = malloc(mesh->indices_size);


	size_t vertex_offset = 0;
	size_t index_offset = 0;
	size_t index = 0;
	for (unsigned int i = 0; i < obj->face_count; ++i){

		for (unsigned int j = 0; j < obj->face_vertices[i]; ++j){

			fastObjIndex gi = obj->indices[index_offset + j];

			vertex_t v;

			v.position[0] = obj->positions[gi.p * 3 + 0];
			v.position[1] = obj->positions[gi.p * 3 + 1];
			v.position[2] = obj->positions[gi.p * 3 + 2];
			//v.normals[0] = obj->normals[gi.n * 3 + 0];
			//v.normals[1] = obj->normals[gi.n * 3 + 1];
			//v.normals[2] = obj->normals[gi.n * 3 + 2];
			v.texcoord[0] = obj->texcoords[gi.t * 2 + 0];
			v.texcoord[1] = 1.0f - obj->texcoords[gi.t * 2 + 1];

			if (j >= 3){
				mesh->vertices[vertex_offset + 0] = mesh->vertices[vertex_offset - 3];
				mesh->vertices[vertex_offset + 1] = mesh->vertices[vertex_offset - 1];
				vertex_offset += 2;
			}
			mesh->vertices[vertex_offset] = v;
			mesh->indices[vertex_offset] = index;

			vertex_offset++;
			index++;
		}
		index_offset += obj->face_vertices[i];
	}
	fast_obj_destroy(obj);
}

cgltf_accessor* LAV_GetAccessor(const cgltf_attribute* attributes, size_t attribute_count, cgltf_attribute_type type){
	int index = 0;
	for (size_t i = 0; i < attribute_count; ++i)
		if (attributes[i].type == type && attributes[i].index == index)
			return attributes[i].data;

	return 0;
}

void LAV_ParseGLTF(const char *path, mesh_t *mesh){

	cgltf_options options = {0};
	cgltf_data* data = 0;
	cgltf_result res = cgltf_parse_file(&options, path, &data);

	if (res != cgltf_result_success){
		printf("Error loading %s\n", path);
	}

	res = cgltf_load_buffers(&options, data, path);
	if (res != cgltf_result_success){

		printf("Error loading %s buffers\n", path);
		cgltf_free(data);
	}

	res = cgltf_validate(data);
	if (res != cgltf_result_success){

		printf("Error validation of %s failed\n", path);
		cgltf_free(data);
	}

	size_t total_vertices = 0;
	size_t total_indices = 0;

	for (size_t ni = 0; ni < data->nodes_count; ++ni){

		if (!data->nodes[ni].mesh)
			continue;

		const cgltf_mesh *a_mesh = data->nodes[ni].mesh;

		for (size_t pi = 0; pi < a_mesh->primitives_count; ++pi){
			const cgltf_primitive *primitive = &a_mesh->primitives[pi];

			cgltf_accessor* ai = primitive->indices;
			cgltf_accessor* ap = LAV_GetAccessor(primitive->attributes, primitive->attributes_count, cgltf_attribute_type_position);

			if (!ai || !ap)
				continue;

			total_vertices += ap->count;
			total_indices += ai->count;
		}
	}

	mesh->vertices = malloc(sizeof(vertex_t) * total_vertices);
	mesh->indices = malloc(sizeof(uint32_t) * total_indices);

	size_t vertex_offset = 0;
	size_t index_offset = 0;

	for (size_t ni = 0; ni < data->nodes_count; ++ni){

		if (!data->nodes[ni].mesh)
			continue;

		const cgltf_mesh *a_mesh = data->nodes[ni].mesh;

		float transform[16];
		cgltf_node_transform_world(&data->nodes[ni], transform);

		for (size_t pi = 0; pi < a_mesh->primitives_count; ++pi){
			const cgltf_primitive *primitive = &a_mesh->primitives[pi];

			cgltf_accessor* ai = primitive->indices;
			cgltf_accessor* ap = LAV_GetAccessor(primitive->attributes, primitive->attributes_count, cgltf_attribute_type_position);

			if (!ai || !ap)
				continue;

			for (size_t i = 0; i < ai->count; ++i){
				mesh->indices[index_offset + i] = (uint32_t)(vertex_offset + cgltf_accessor_read_index(ai, i));

				for (size_t i = 0; i < ap->count; ++i){

					float ptr[3];
					cgltf_accessor_read_float(ap, i, ptr, 3);

					mesh->vertices[vertex_offset + i].position[0] = ptr[0] * transform[0] + ptr[1] * transform[4] + ptr[2] * transform[8] + transform[12];
					mesh->vertices[vertex_offset + i].position[1] = ptr[0] * transform[1] + ptr[1] * transform[5] + ptr[2] * transform[9] + transform[13];
					mesh->vertices[vertex_offset + i].position[2] = ptr[0] * transform[2] + ptr[1] * transform[6] + ptr[2] * transform[10] + transform[14];
				}
			}
			/*
			cgltf_accessor* an = LAV_GetAccessor(primitive->attributes, primitive->attributes_count, cgltf_attribute_type_normal);

			if (an){
				for (size_t i = 0; i < ap->count; ++i){
					float ptr[3];
					cgltf_accessor_read_float(an, i, ptr, 3);

					mesh->vertices[vertex_offset + i].normals[0] = ptr[0] * transform[0] + ptr[1] * transform[4] + ptr[2] * transform[8];
					mesh->vertices[vertex_offset + i].normals[1] = ptr[0] * transform[1] + ptr[1] * transform[5] + ptr[2] * transform[9];
					mesh->vertices[vertex_offset + i].normals[2] = ptr[0] * transform[2] + ptr[1] * transform[6] + ptr[2] * transform[10];
				}
			}
			*/
			cgltf_accessor* at = LAV_GetAccessor(primitive->attributes, primitive->attributes_count, cgltf_attribute_type_texcoord);

			if (at){

				for (size_t i = 0; i < ap->count; ++i){
					float ptr[2];
					cgltf_accessor_read_float(at, i, ptr, 2);

					mesh->vertices[vertex_offset + i].texcoord[0] = ptr[0];
					mesh->vertices[vertex_offset + i].texcoord[1] = ptr[1];
				}
			}

			vertex_offset += ap->count;
			index_offset += ai->count;
		}
	}
	cgltf_free(data);
}

void LAV_DestroyMesh(mesh_t *mesh){
	free(mesh->vertices);
	free(mesh->indices);
}


void LAV_LoadModel(LAV_Context *ctx, const char* path)
{
	if (strstr(path, ".obj")){
		mesh_t model;
		model.vertices = NULL;
		model.indices = NULL;
		LAV_ParseOBJ(path, &model);
		LAV_CreateVertexBuffer(ctx, &model);
		LAV_CreateIndexBuffer(ctx, &model);
		//TODO
		ctx->vertices = model.vertices_size / sizeof(vertex_t);
		//printf("%f %f %f\n", model.vertices[0].position[0],  model.vertices[0].position[1], model.vertices[0].position[2]);
		LAV_DestroyMesh(&model);
	};

	if (strstr(path, ".gltf") || strstr(path, ".glb")){
		mesh_t model;
		model.vertices = NULL;
		model.indices = NULL;
		LAV_ParseGLTF(path, &model);
		LAV_CreateVertexBuffer(ctx, &model);
		LAV_CreateIndexBuffer(ctx, &model);
		//printf("%f %f %f\n", model.vertices[0].position[0],  model.vertices[0].position[1], model.vertices[0].position[2]);
		LAV_DestroyMesh(&model);
	};
}


