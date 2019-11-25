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
	VK_MIN_FRAMETIME = 16, //60fps
	VK_MAX_VERTEX_ATTRIBUTES = 16
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

	VK_Context* ctx = VK_CreateContext(window, window_title,
			NUM(instance_layers),
			instance_layers,
			NUM(device_extensions),
			device_extensions,
			VK_CONTEXT_DEBUG | VK_CONTEXT_MIPMAPS | VK_CONTEXT_MULTISAMPLING);
	
	/*
	lav_attr_t atributes[] = 
	*/
	/*
	lav_shader_d flat_textured = {0};
	flat_textured.vs_source = "./assets/shaders/vert.spv";
	flat_textured.fs_source = "./assets/shaders/frag.spv";


	lav_pipeline pip = VK_CreatePipeline(&pip_description); 

	lav_


	*/

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
		if(elapsed_time < VK_MIN_FRAMETIME){
			// Not enough time has elapsed. Let's limit the frame rate
			SDL_Delay(VK_MIN_FRAMETIME - elapsed_time);
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

		vkWaitForFences(ctx->device, 1, &ctx->in_flight_fence[current_frame], VK_TRUE, UINT64_MAX);

		uint32_t image_index;
		VkResult result = vkAcquireNextImageKHR(ctx->device, ctx->swapchain, UINT64_MAX, ctx->image_available_semaphore[current_frame], VK_NULL_HANDLE, &image_index);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			VK_RecreateSwapchain(ctx, window);
			goto REDRAW;
		} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			printf("failed to acquire swap chain image!\n");
		}
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
		vkMapMemory(ctx->device, ctx->uniform_buffer_allocation[current_frame], 0, sizeof(ubo_t), 0, &temp_data);
		memcpy(temp_data, &mvp, sizeof(ubo_t));
		vkUnmapMemory(ctx->device, ctx->uniform_buffer_allocation[current_frame]);

		VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO};
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &ctx->image_available_semaphore[current_frame]; // wait_semaphores,
		submit_info.pWaitDstStageMask = &(VkPipelineStageFlags){VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}; // wait_stages,
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &ctx->command_buffers[image_index];
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
			VK_RecreateSwapchain(ctx, window);
		} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			printf("failed to acquire swap chain image!\n");
		}

		current_frame = (current_frame + 1) % 2;

	}

	VK_DestroyContext(ctx);
	SDL_Quit();
	exit(EXIT_SUCCESS);
}
