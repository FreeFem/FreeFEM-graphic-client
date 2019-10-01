/**
 * @file NativeWindow.h
 * @brief Minimal abstraction of a GLFW window.
 */
#ifndef NATIVE_WINDOW_H_
#define NATIVE_WINDOW_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

namespace ffGraph {
namespace Vulkan {

struct NativeWindow {
    GLFWwindow* Handle = NULL;
    VkExtent2D WindowSize = {1280, 768};
    std::vector<std::string> SurfaceExtensions = {};
};

/**
 * @brief Check if the ffGraph::Vulkan::NativeWindow is ready to be used.
 *
 * @param Window [in] - ffGraph::Vulkan::NativeWindow tested.
 *
 * @return bool
 */
inline bool ffIsNativeWindowReady(NativeWindow Window) { return (Window.Handle == NULL) ? false : true; }

/**
 * @brief Create a new ffGraph::Vulkan::NativeWindow.
 *
 * @param Dimensions [in] - Window width and height.
 *
 * @return ffGraph::Vulkan::NativeWindow - Use ffGraph::Vulkan::ffIsNativeWindowReady() to check return value.
 */
NativeWindow ffNewWindow(VkExtent2D Dimensions);

/**
 * @brief Destroy a ffGraph::Vulkan::NativeWindow.
 *
 * @param Window - ffGraph::Vulkan::NativeWindow to destroy.
 *
 * @return void
 */
void ffDestroyWindow(NativeWindow Window);

/**
 * @brief Call mandatory function : glfwInit().
 *
 * return bool
 */
inline bool ffInitGFLW( ) { return glfwInit( ); }

/**
 * @brief Call mandatory function : glfwTerminate().
 *
 * return void
 */
inline void ffTerminateGLFW( ) { glfwTerminate( ); }

/**
 * @brief Use GLFW to get our Vulkan VkSurfaceKHR.
 *
 * @param Instance [in] - Vulkan VkInstance used to create the VkSurfaceKHR.
 * @param Window [in] - ffGraph::Vulkan::NativeWindow used to create the VkSurfaceKHR.
 *
 * @return VkSurfaceKHR - Returns VK_NULL_HANDLE if function failed.
 */
VkSurfaceKHR ffGetSurface(const VkInstance& Instance, const NativeWindow& Window);

/**
 * @brief Check if ffGraph::Vulkan::NativeWindow::Handle should close.
 *
 * @param Window - ffGraph::Vulkan::NativeWindow get the event.
 *
 * @return int - See GLFW documentation for function glfwWindowShouldClose().
 */
inline int ffWindowShouldClose(NativeWindow Window) {
    glfwPollEvents( );
    return glfwWindowShouldClose(Window.Handle);
}

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // NATIVE_WINDOW_H_