#include "platform/vulkan_platform.h"

// Linux platform layer.
#if KPLATFORM_LINUX

#    include <xcb/xcb.h>

// For surface creation
#    define VK_USE_PLATFORM_XCB_KHR
#    include <vulkan/vulkan.h>

#    include <containers/darray.h>
#    include <logger.h>
#    include <memory/kmemory.h>
#    include <platform/platform.h>

#    include "vulkan_types.h"

typedef struct linux_handle_info {
    xcb_connection_t* connection;
    xcb_screen_t* screen;
} linux_handle_info;

typedef struct kwindow_platform_state {
    xcb_window_t window;
} kwindow_platform_state;

void vulkan_platform_get_required_extension_names(const char*** names_darray) {
    darray_push(*names_darray, &"VK_KHR_xcb_surface"); // VK_KHR_xlib_surface?
}

// Surface creation for Vulkan
b8 vulkan_platform_create_vulkan_surface(vulkan_context* context, struct kwindow* window) {
    u64 size = 0;
    platform_get_handle_info(&size, 0);
    void* block = kallocate(size, MEMORY_TAG_RENDERER);
    platform_get_handle_info(&size, block);

    linux_handle_info* handle = (linux_handle_info*)block;

    VkXcbSurfaceCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};
    create_info.connection = handle->connection;
    create_info.window = window->platform_state->window;

    VkResult result = vkCreateXcbSurfaceKHR(
        context->instance,
        &create_info,
        context->allocator,
        &window->renderer_state->backend_state->surface);
    if (result != VK_SUCCESS) {
        KFATAL("Vulkan surface creation failed.");
        return false;
    }

    return true;
}

b8 vulkan_platform_presentation_support(vulkan_context* context, VkPhysicalDevice physical_device, u32 queue_family_index) {
    u64 size = 0;
    platform_get_handle_info(&size, 0);
    void* block = kallocate(size, MEMORY_TAG_RENDERER);
    platform_get_handle_info(&size, block);

    linux_handle_info* handle = (linux_handle_info*)block;

    return (b8)vkGetPhysicalDeviceXcbPresentationSupportKHR(physical_device, queue_family_index, handle->connection, handle->screen->root_visual);
}

#endif
