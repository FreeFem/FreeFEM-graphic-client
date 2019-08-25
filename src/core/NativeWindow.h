#ifndef NATIVE_WINDOW_H
#define NATIVE_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "../util/utils.h"

class NativeWindow {
    public:
        FORCE_USE_RESULT Error init(uint32_t width, uint32_t height, const char *title);
        void destroy();

        inline uint32_t getWidth() const { return m_width; }
        inline uint32_t getHeight() const { return m_height; }
        inline void setWidth(int width) { m_width = width; }
        inline void setHeight(int height) {m_height = height; }

        GLFWwindow *getNativeWindow() const;

        bool isUsable() const;

        std::vector<const char *> getNativeExtensions() const;

    private:
        GLFWwindow *m_window = NULL;
        uint32_t m_width;
        uint32_t m_height;
        std::vector<const char *> m_extensions;

};

#endif // NATIVE_WINDOW_H