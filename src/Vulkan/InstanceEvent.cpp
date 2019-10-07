#include "Instance.h"
#include "utils.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

static void FramebufferResizeCallback(GLFWwindow *Window, int width, int height)
{
    Instance *Handle = static_cast<Instance *>(glfwGetWindowUserPointer(Window));

    Handle->m_Window.WindowSize = {(uint32_t)width, (uint32_t)height};
    Handle->reload();
}

static void KeyCallback(GLFWwindow *Window, int key, UNUSED_PARAM(int scancode), int action, UNUSED_PARAM(int mods)) {
    Instance *Handle = static_cast<Instance *>(glfwGetWindowUserPointer(Window));

    if (key == GLFW_KEY_KP_4) {
        Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model = glm::rotate(Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model, glm::radians(2.f), glm::vec3(0.f, 1.f, 0.f));
    } else if (key == GLFW_KEY_KP_6) {
        Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model = glm::rotate(Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model, glm::radians(2.f), glm::vec3(0.f, -1.f, 0.f));
    } else if (key == GLFW_KEY_KP_8) {
        Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model = glm::rotate(Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model, glm::radians(2.f), glm::vec3(1.f, 0.f, 0.f));
    } else if (key == GLFW_KEY_KP_2) {
        Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model = glm::rotate(Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model, glm::radians(2.f), glm::vec3(-1.f, 0.f, 0.f));
    } else if (key == GLFW_KEY_KP_7) {
        Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model = glm::rotate(Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model, glm::radians(2.f), glm::vec3(0.f, 0.f, -1.f));
    } else if (key == GLFW_KEY_KP_9) {
        Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model = glm::rotate(Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model, glm::radians(2.f), glm::vec3(0.f, 0.f, 1.f));
    }
    if (action == GLFW_PRESS) {
        CameraKeyEvents(Handle->Graphs[Handle->CurrentRenderGraph].Cam, key);
    }
    Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.ViewProj = Handle->Graphs[Handle->CurrentRenderGraph].Cam.Handle.ViewProjMatrix;
}

static void MouseScroolCallback(GLFWwindow *Window, UNUSED_PARAM(double xOffset), double yOffset) {
    Instance *Handle = static_cast<Instance *>(glfwGetWindowUserPointer(Window));

    CameraScroolEvents(Handle->Graphs[Handle->CurrentRenderGraph].Cam, yOffset);
    Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.ViewProj = Handle->Graphs[Handle->CurrentRenderGraph].Cam.Handle.ViewProjMatrix;
}

static void MousePressCallback(GLFWwindow *Window, int button, int action, UNUSED_PARAM(int mods))
{
    Instance *Handle = static_cast<Instance *>(glfwGetWindowUserPointer(Window));

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(Window, &Handle->m_Window.MouseX, &Handle->m_Window.MouseY);
    }
}

void Instance::initGFLWCallbacks()
{
    glfwSetInputMode(m_Window.Handle, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetKeyCallback(m_Window.Handle, &KeyCallback);
    glfwSetScrollCallback(m_Window.Handle, &MouseScroolCallback);
    glfwSetMouseButtonCallback(m_Window.Handle, &MousePressCallback);
}

void Instance::Events()
{
    if (glfwGetMouseButton(m_Window.Handle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double cMouseX, cMouseY;
        glfwGetCursorPos(m_Window.Handle, &cMouseX, &cMouseY);

        if (m_Window.MouseX != cMouseX || m_Window.MouseY != cMouseY) {
            float x = cMouseX - m_Window.MouseX;
            float y = cMouseY - m_Window.MouseY;

            Graphs[CurrentRenderGraph].PushCamera.Model = glm::rotate(Graphs[CurrentRenderGraph].PushCamera.Model, glm::radians(y * 0.25f), glm::vec3(1.f, 0.f, 0.f));
            Graphs[CurrentRenderGraph].PushCamera.Model = glm::rotate(Graphs[CurrentRenderGraph].PushCamera.Model, glm::radians(x * 0.25f), glm::vec3(0.f, 1.0f, 0.f));
            m_Window.MouseX = cMouseX;
            m_Window.MouseY = cMouseY;
        }


    }
}

}
}