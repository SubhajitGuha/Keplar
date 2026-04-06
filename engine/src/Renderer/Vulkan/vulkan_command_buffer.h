#pragma once
#include "vulkan_types.inl"

void vulkan_command_buffer_allocate(vulkan_context* context,
VkCommandPool pool,
b8 is_primary,
vulkan_command_buffer* out_cmd_buffer);

void vulkan_command_buffer_free(vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* cmd_buffer);

void vulkan_command_buffer_begin(
    vulkan_command_buffer* cmd_buffer,
    b8 is_single_use,
    b8 is_simultaneous_use
);

void vulkan_command_buffer_end(vulkan_command_buffer* cmd_buffer);

void vulkan_command_buffer_update_submitted(vulkan_command_buffer* cmd_buffer);

void vulkan_command_buffer_reset(vulkan_command_buffer* cmd_buffer);

/**
 * Allocates and begins recording to out_command_buffer.
 */
void vulkan_command_buffer_allocate_and_begin_single_use(
    vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* out_cmd_buffer);

/**
 * Ends recording, submits to and waits for queue operation and frees the provided command buffer.
 */
void vulkan_command_buffer_end_single_use(
    vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* cmd_buffer,
    VkQueue queue);