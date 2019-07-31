#include <GLFW/glfw3.h>
#include <volk.h>

struct sWindow {
    GLFWwindow *Window;
    struct {
        uint32_t width;
        uint32_t height;
    } Dimensions;

    VkSurfaceKHR Surface;
    VkQueue PresentationQueue;
};

/**
 * @brief Create a new window
 *
 * @param uint32_t width - width of the window
 * @param uint32_t height - height of the window
 * @param const char *title - title of the window
 * @param sWindow *window - Window struct to be filled
 */
bool createWindow(uint32_t width, uint32_t height, const char *title, sWindow *window);

/**
 * @brief Destroy a window, freeing memory
 *
 * @param sWindow window - sWindow to destroy
 */
void destroyWindow(sWindow window);