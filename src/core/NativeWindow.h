#ifndef NATIVE_WINDOW_H
#define NATIVE_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "../util/utils.h"

/**
 * @brief Wrapper around GFLW for a easier use.
 */
class NativeWindow {
    public:
        /**
         * @brief Initialize a new GLFW window.
         *
         * @param uint32_t width[in] - Window's width.
         * @param uint32_t height[in] - Window's height.
         * @param const char *title[in] - Window's title.
         */
        FORCE_USE_RESULT Error init(uint32_t width, uint32_t height, const char *title);

        /**
         * @brief Destroy the window.
         */
        void destroy();

        /**
         * @brief Window's width getter.
         *
         * @return uint32_t - Window's width.
         */
        inline uint32_t getWidth() const { return m_width; }

        /**
         * @brief Window's height getter.
         *
         * @return uint32_t - Window's height.
         */
        inline uint32_t getHeight() const { return m_height; }

        /**
         * @brief Window's width setter.
         *
         * @param int width[in] - New window's width.
         */
        inline void setWidth(int width) { m_width = width; }

        /**
         * @brief Window's height setter.
         *
         * @param int height[in] - New window's height.
         */
        inline void setHeight(int height) {m_height = height; }
        /**
         * @brief Window's size setter.
         *
         * @param int width[in] - New window's width.
         * @param int height[in] - New window's height.
         */
        inline void setSize(int width, int height) { m_width = width; m_height = height; }

        /**
         * @brief GLFWwindow handle getter.
         *
         * @return GLFWwindow * - GLFWwindow handle.
         */
        GLFWwindow *getNativeWindow() const;

        /**
         * @brief Look if window was correctly created.
         */
        bool isUsable() const;

        /**
         * @brief Get GLFWwindow vulkan extensions (platform specific).
         *
         * @return std::vector<const char *> - Vulkan extensions.
         */
        std::vector<const char *> getNativeExtensions() const;

    private:
        /**
         * @brief GLFW handle.
         */
        GLFWwindow *m_window = NULL;
        /**
         * @brief Window's width.
         */
        uint32_t m_width;
        /**
         * @brief Window's height.
         */
        uint32_t m_height;
        /**
         * @brief GLFW vulkan extensions.
         */
        std::vector<const char *> m_extensions;

};

#endif // NATIVE_WINDOW_H