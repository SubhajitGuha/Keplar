#include "vulkan_command_buffer.h"
#include "core/kmemory.h"

void vulkan_command_buffer_allocate(vulkan_context* context,
VkCommandPool pool,
b8 is_primary,
vulkan_command_buffer* out_cmd_buffer)
{
    kzero_memory(out_cmd_buffer, sizeof(out_cmd_buffer));

    VkCommandBufferAllocateInfo ai = {};
    ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    ai.commandPool = pool;
    ai.level = is_primary? VK_COMMAND_BUFFER_LEVEL_PRIMARY:VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    ai.commandBufferCount = 1; //for now its 1
    ai.pNext = 0;

    out_cmd_buffer->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
    VK_CHECK(vkAllocateCommandBuffers(context->device.logical_device, &ai, out_cmd_buffer->handle));

    out_cmd_buffer->state = COMMAND_BUFFER_STATE_READY;
}

void vulkan_command_buffer_free(vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* cmd_buffer)
    {
        vkFreeCommandBuffers(context->device.logical_device, pool, 1, &cmd_buffer->handle);

        //zero out the memory
        cmd_buffer->handle = 0;
        cmd_buffer->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
    }

void vulkan_command_buffer_begin(
    vulkan_command_buffer* cmd_buffer,
    b8 is_single_use,
    b8 is_simultaneous_use
)
{
    VkCommandBufferBeginInfo cb_bi = {};
    cb_bi.sType= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cb_bi.flags = 0;
    if(is_single_use)
        cb_bi.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if(is_simultaneous_use)
        cb_bi.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    VK_CHECK(vkBeginCommandBuffer(cmd_buffer->handle,&cb_bi));
    cmd_buffer->state = COMMAND_BUFFER_STATE_RECORDING;
}

void vulkan_command_buffer_end(vulkan_command_buffer* cmd_buffer)
{
    VK_CHECK(vkEndCommandBuffer(cmd_buffer));
    cmd_buffer->state = COMMAND_BUFFER_STATE_RECORDING_ENDED;
}

void vulkan_command_buffer_update_submitted(vulkan_command_buffer* cmd_buffer)
{
    cmd_buffer->state = COMMAND_BUFFER_STATE_READY;
}

void vulkan_command_buffer_reset(vulkan_command_buffer* cmd_buffer)
{
    cmd_buffer->state = COMMAND_BUFFER_STATE_READY;
}

/**
 * Allocates and begins recording to out_command_buffer.
 */
void vulkan_command_buffer_allocate_and_begin_single_use(
    vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* out_cmd_buffer)
    {
        vulkan_command_buffer_allocate(context,pool,TRUE,out_cmd_buffer);
        vulkan_command_buffer_begin(out_cmd_buffer, TRUE, FALSE);
    }

/**
 * Ends recording, submits to and waits for queue operation and frees the provided command buffer.
 */
void vulkan_command_buffer_end_single_use(
    vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* cmd_buffer,
    VkQueue queue)
    {
        vulkan_command_buffer_end(cmd_buffer);

        // End the command buffer.
    vulkan_command_buffer_end(cmd_buffer);

    // Submit the queue
    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buffer->handle;
    VK_CHECK(vkQueueSubmit(queue, 1, &submit_info, 0));

    // Wait for it to finish
    VK_CHECK(vkQueueWaitIdle(queue));

    // Free the command buffer.
    vulkan_command_buffer_free(context, pool, cmd_buffer);
    }