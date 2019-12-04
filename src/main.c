#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>
#include <SDL2/SDL_vulkan.h>

#include "SDL_lava.h"
#include "SDL_lava_import.h"


#define HANDMADE_MATH_IMPLEMENTATION
//#define HANDMADE_MATH_DEPTH_ZERO_TO_ONE
#include "HandmadeMath.h"

#define NUM(a) (sizeof(a)/sizeof(a[0]))

enum{
	LAV_MIN_FRAMETIME = 16, //60fps
};

int main(void){

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Vulkan_LoadLibrary(NULL);

	const char *window_title = "Hello Vulkan";

	SDL_Window *window = SDL_CreateWindow(window_title,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			800,
			600,
			SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE); 

	const char *instance_layers[] = {"VK_LAYER_LUNARG_standard_validation"};
	const char *device_extensions[] = {"VK_KHR_swapchain"};

	LAV_Context* ctx = LAV_CreateContext(window, window_title,
			NUM(instance_layers), instance_layers,
			NUM(device_extensions),	device_extensions,
			LAV_CTX_DEBUG | LAV_CTX_MIPMAPS | LAV_CTX_MULTISAMPLING);

	LAV_UniformBuffer *ubo = LAV_CreateUniformBuffer(ctx, sizeof(ubo_t));
	LAV_Texture *tex = LAV_CreateTexture(ctx, "../assets/images/chalet.jpg"); 




	VkDescriptorSetLayoutBinding description_set_bindigns[] = {
		LAV_CreateBindingDescriptor(0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		LAV_CreateBindingDescriptor(1, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
	};

	/*	
	lav_descriptor descriptors[] {
		LAV_SetUniformBuffer(ubo, 0);
		LAV_SetCombinedImageSapler(tex, 1);
	}

	lav_vertex_input vert[] {
		LAV_
	}

	lav_atributes atributes[]
	lav_pusch_constants[]
	lav_indices[]


	*/

	LAV_PipelineLayout *layout = LAV_CreatePipelineLayout(ctx,
			NUM(description_set_bindigns), description_set_bindigns,
			0, NULL);

	const char *flat_texture_shaders[] = {"../assets/shaders/flat_texture.vert.spv", "../assets/shaders/flat_texture.frag.spv" };

	VkVertexInputBindingDescription input_description[] = {
		LAV_CreateVertexInputDescriptor(0, 5 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX)
	};
	
	VkVertexInputAttributeDescription attribute_description[] = {
		LAV_CreateShaderDescriptor(0,0, VK_FORMAT_R32G32B32_SFLOAT, 0),
		LAV_CreateShaderDescriptor(0,1, VK_FORMAT_R32G32_SFLOAT, 3 * sizeof(float))
	};

	LAV_Pipeline *pip = LAV_CreatePipeline(ctx, layout,
			NUM(flat_texture_shaders), flat_texture_shaders,
			NUM(input_description), input_description,
			NUM(attribute_description),	attribute_description,
			0);


	//Rest
	LAV_Rest(ctx, layout, pip, ubo, tex);	

	mesh_t model;
	model.vertices = NULL;
	model.indices = NULL;
	LAV_ParseOBJ("../assets/models/chalet.obj", &model);

	LAV_VertexBuffer *vbo = LAV_CreateVertexBuffer(ctx, &model);
	LAV_IndexBuffer *ibo = LAV_CreateIndexBuffer(ctx, &model);
	//TODO
	ibo->indices = model.vertices_size / sizeof(vertex_t);

	VkDeviceSize offsets[] = {0};

	LavCommand commands[] = {
		LAV_BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pip->graphics_pipeline),	
		LAV_BindVertexBuffer(0,1, &vbo->vertex_buffer, offsets),
		LAV_BindDescriptors(VK_PIPELINE_BIND_POINT_GRAPHICS, layout->pipeline_layout, 0, 1, ctx->descriptor_sets, 0, NULL),
		LAV_Draw(ibo->indices, 1, 0, 0)
		//LAV_BindIndexBuffer(ctx->index_buffer, 0, VK_INDEX_TYPE_UINT32),
		//LAV_DrawIndexed(ctx->vertices, 1, 0, 0, 0)
	};

	LAV_CommandBuffer *cbo = LAV_CreateCommandBuffers(ctx, NUM(commands), commands);

	
	float rotation_angle = 0.0f;	
	float rotation_speed = 20.0f;
	uint32_t prev_time = SDL_GetTicks();
	uint32_t current_time, elapsed_time;

	size_t current_frame = 0;

	bool framebuffer_resized = false;

	bool quit = false;

	SDL_Event event;
	while( !quit ){

		REDRAW:
		//Framerate independent animation
		current_time = SDL_GetTicks();
		elapsed_time = current_time - prev_time;
		//Framerate limit
		if(elapsed_time < LAV_MIN_FRAMETIME){
			// Not enough time has elapsed. Let's limit the frame rate
			SDL_Delay(LAV_MIN_FRAMETIME - elapsed_time);
			current_time = SDL_GetTicks();
			elapsed_time = current_time - prev_time;
		}
		prev_time = current_time;


		while( SDL_PollEvent( &event ) != 0 ){
			switch (event.type) {
				 case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
						framebuffer_resized = true;
					}
					break;
				case SDL_QUIT:
					quit = true;
					break;

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							quit = true;
							break;
						default:
							break;
					}
					break;
				default:
					break;

			}
		}

		//Update bindings
		ubo_t mvp;
		mvp.model = HMM_Mat4d(1.0f);
		rotation_angle -= rotation_speed * (float)elapsed_time / 1000.0f;
		mvp.model = HMM_Rotate(rotation_angle, (hmm_vec3){{0.0f,0.0f,1.0f}});

		hmm_vec3 camera = {{2.0f, 2.0f,2.0f}};
		hmm_vec3 world_origin = {{0.0f, 0.0f, 0.0f}};
		hmm_vec3 up_direction = {{0.0f,0.0f, 1.0f}};

		mvp.view = HMM_LookAt(camera, world_origin, up_direction);

		float fov = 45.0f;
		float aspect_ratio = (float)ctx->window_width / (float)ctx->window_height;
		float near_plane = 0.1f;
		float far_plane = 10.0f;

		mvp.proj = HMM_Perspective(fov, aspect_ratio, near_plane, far_plane);
		mvp.proj.Elements[1][1] *= -1.0f;

		void* temp_data; // = malloc(buffer_size);
		vkMapMemory(ctx->device, ubo->uniform_buffer_allocation[current_frame], 0, sizeof(ubo_t), 0, &temp_data);
		memcpy(temp_data, &mvp, sizeof(ubo_t));
		vkUnmapMemory(ctx->device, ubo->uniform_buffer_allocation[current_frame]);


		vkWaitForFences(ctx->device, 1, &ctx->in_flight_fence[current_frame], VK_TRUE, UINT64_MAX);

		uint32_t image_index;
		VkResult result = vkAcquireNextImageKHR(ctx->device, ctx->swapchain, UINT64_MAX, ctx->image_available_semaphore[current_frame], VK_NULL_HANDLE, &image_index);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			LAV_RecreateSwapchain(ctx, window, layout, pip, tex, ubo, cbo);
			goto REDRAW;
		} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			printf("failed to acquire swap chain image!\n");
		}
	
		VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO};
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &ctx->image_available_semaphore[current_frame]; // wait_semaphores,
		submit_info.pWaitDstStageMask = &(VkPipelineStageFlags){VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}; // wait_stages,
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &cbo->command_buffers[image_index];
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &ctx->render_finished_semaphore[current_frame]; // signal_semaphores

		vkResetFences(ctx->device, 1, &ctx->in_flight_fence[current_frame]);
		assert(vkQueueSubmit(ctx->device_queue, 1, &submit_info, ctx->in_flight_fence[current_frame]) == VK_SUCCESS);

		VkPresentInfoKHR present_info = {.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &ctx->render_finished_semaphore[current_frame]; // signal_semaphores,
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &ctx->swapchain;
		present_info.pImageIndices = &image_index;
		present_info.pResults = NULL; // Optional

		//assuming present ation and graphics queues are the same one
		result = vkQueuePresentKHR(ctx->device_queue, &present_info);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebuffer_resized) {
			framebuffer_resized = false;
			LAV_RecreateSwapchain(ctx, window, layout, pip, tex, ubo, cbo);
		} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			printf("failed to acquire swap chain image!\n");
		}

		current_frame = (current_frame + 1) % 2;

	}

	LAV_DestroyContext(ctx, layout, pip, tex, ubo, vbo, ibo,  cbo);
	SDL_Quit();
	exit(EXIT_SUCCESS);
}
