#pragma once

#include "SDL_lava.h"

typedef enum{
	VK_CMD_BEGIN_RENDER_PASS,
	VK_CMD_BIND_PIPELINE,
	VK_CMD_BIND_VERTEX_BUFFER,
	VK_CMD_BIND_INDEX_BUFFER,
	VK_CMD_BIND_DESCRIPTOR_SET,
	VK_CMD_DRAW,
	VK_CMD_DRAW_INDEXED,
	VK_CMD_END_RENDER_PASS
}cmd_t;

typedef union{

}commands_t;

void VK_CreateCommandBuffers(VK_Context *ctx);
