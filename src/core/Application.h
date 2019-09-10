#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "../vk/Pipeline.h"
#include "../vk/ShaderLoader.h"
#include "../vk/VulkanContext.h"
#include "Window.h"

namespace FEM {

struct ApplicationCreateInfo {
    uint32_t ScreenWidth;
    uint32_t ScreenHeight;
};

struct Application {
    Window Screen;

    VK::VulkanContext vkContext;
    VK::Pipeline Renderer;
    VK::ShaderLoader Shaders;
};

ApplicationCreateInfo getApplicationInfos(int ac, char **av);

bool newApplication(Application *App, const ApplicationCreateInfo AppCreateInfos);

void destroyApplication(Application *App);

void runApplication(Application *App);

bool renderCurrent(VK::VulkanContext &vkContext, const VK::Pipeline Renderer, const Window Win);

}    // namespace FEM

#endif    // APPLICATION_H_