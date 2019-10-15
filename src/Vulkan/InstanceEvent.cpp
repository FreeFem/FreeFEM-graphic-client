#include <imgui.h>
#include "Instance.h"
#include "utils.h"
#include "Logger.h"
#include "Resource/Mesh/Mesh.h"

namespace ffGraph {
namespace Vulkan {

static void FramebufferResizeCallback(GLFWwindow *Window, int width, int height) {
    Instance *Handle = static_cast<Instance *>(glfwGetWindowUserPointer(Window));
    ImGuiIO &io = ImGui::GetIO( );

    io.DisplaySize = ImVec2(width, height);
    Handle->m_Window.WindowSize = {(uint32_t)width, (uint32_t)height};
    for (auto &Graph : Handle->Graphs) {
        Graph.Update = true;
        for (auto &Node : Graph.Nodes) {
            Node.Update = true;
        }
    }
    Handle->reload( );
}

int FindArraySurface(std::vector<RenderGraphNode> Nodes) {
    int i = 0;
    for (auto &Node : Nodes) {
        if (Node.GeoType == JSON::GeometryType::Surface) {
            return i;
        }
        ++i;
    }
    return -1;
}

static void Imgui_Key(int key, int action) {
    ImGuiIO &io = ImGui::GetIO( );

    if (action == GLFW_PRESS) io.KeysDown[key] = true;
    if (action == GLFW_RELEASE) io.KeysDown[key] = false;

    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

static void KeyCallback(GLFWwindow *Window, int key, UNUSED_PARAM(int scancode), int action, UNUSED_PARAM(int mods)) {
    Instance *Handle = static_cast<Instance *>(glfwGetWindowUserPointer(Window));

    Imgui_Key(key, action);
    if (key == GLFW_KEY_KP_4) {
        Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model = glm::rotate(
            Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model, glm::radians(2.f), glm::vec3(0.f, 1.f, 0.f));
    } else if (key == GLFW_KEY_KP_6) {
        Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model = glm::rotate(
            Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model, glm::radians(2.f), glm::vec3(0.f, -1.f, 0.f));
    } else if (key == GLFW_KEY_KP_8) {
        Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model = glm::rotate(
            Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model, glm::radians(2.f), glm::vec3(1.f, 0.f, 0.f));
    } else if (key == GLFW_KEY_KP_2) {
        Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model = glm::rotate(
            Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model, glm::radians(2.f), glm::vec3(-1.f, 0.f, 0.f));
    } else if (key == GLFW_KEY_KP_7) {
        Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model = glm::rotate(
            Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model, glm::radians(2.f), glm::vec3(0.f, 0.f, -1.f));
    } else if (key == GLFW_KEY_KP_9) {
        Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model = glm::rotate(
            Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.Model, glm::radians(2.f), glm::vec3(0.f, 0.f, 1.f));
    }
    if (action == GLFW_PRESS) {
        CameraKeyEvents(Handle->Graphs[Handle->CurrentRenderGraph].Cam, key);
        if (key == GLFW_KEY_V) {
            for (auto &Node : Handle->Graphs[Handle->CurrentRenderGraph].Nodes) {
                if (Node.GeoType == JSON::GeometryType::Volume) {
                    std::cout << "Removing one Node from the rendering queue.\n";
                    Node.to_render = !Node.to_render;
                }
            }
        } else if (key == GLFW_KEY_S) {
            for (auto &Node : Handle->Graphs[Handle->CurrentRenderGraph].Nodes) {
                if (Node.GeoType == JSON::GeometryType::Surface) {
                    std::cout << "Removing one Node from the rendering queue.\n";
                    Node.to_render = !Node.to_render;
                }
            }
        } else if (key == GLFW_KEY_L) {
            for (auto &Node : Handle->Graphs[Handle->CurrentRenderGraph].Nodes) {
                Node.PolygonMode =
                    (Node.PolygonMode == VK_POLYGON_MODE_LINE) ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE;
                Node.Update = true;
            }
            Handle->Graphs[Handle->CurrentRenderGraph].Update = true;
            RenderGraphCreateInfos CreateInfos;
            CreateInfos.Device = Handle->Env.GPUInfos.Device;
            CreateInfos.RenderPass = Handle->Env.GraphManager.RenderPass;
            CreateInfos.msaaSamples = Handle->Env.GPUInfos.Capabilities.msaaSamples;
            CreateInfos.PushConstantPTR = 0;
            CreateInfos.PushConstantSize = 0;
            CreateInfos.Stage = 0;
            CreateInfos.AspectRatio = GetAspectRatio(Handle->m_Window);
            ReloadRenderGraph(CreateInfos, Handle->Graphs[Handle->CurrentRenderGraph]);
        } else if (key == GLFW_KEY_A) {
            if (Handle->CurrentRenderGraph == 0) return;
            Handle->CurrentRenderGraph -= 1;
        } else if (key == GLFW_KEY_D) {
            Handle->CurrentRenderGraph =
                std::min((unsigned long int)Handle->CurrentRenderGraph + 1, Handle->Graphs.size( ) - 1);
        }
    }
    Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.ViewProj =
        Handle->Graphs[Handle->CurrentRenderGraph].Cam.Handle.ViewProjMatrix;
}

static void MouseScroolCallback(GLFWwindow *Window, UNUSED_PARAM(double xOffset), double yOffset) {
    Instance *Handle = static_cast<Instance *>(glfwGetWindowUserPointer(Window));

    CameraScroolEvents(Handle->Graphs[Handle->CurrentRenderGraph].Cam, yOffset);
    Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.ViewProj =
        Handle->Graphs[Handle->CurrentRenderGraph].Cam.Handle.ViewProjMatrix;
}

static void imgui_Mouse(Instance *Instance, int button, int action) {
    if (action == GLFW_PRESS && button >= 0 && button < 5) {
        Instance->PressedButton[button] = true;
    }
}

static void MousePressCallback(GLFWwindow *Window, int button, int action, UNUSED_PARAM(int mods)) {
    Instance *Handle = static_cast<Instance *>(glfwGetWindowUserPointer(Window));

    imgui_Mouse(Handle, button, action);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(Window, &Handle->m_Window.MouseX, &Handle->m_Window.MouseY);
    }
}

void Instance::initGFLWCallbacks( ) {
    glfwSetWindowUserPointer(m_Window.Handle, this);
    glfwSetInputMode(m_Window.Handle, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetKeyCallback(m_Window.Handle, &KeyCallback);
    glfwSetScrollCallback(m_Window.Handle, &MouseScroolCallback);
    glfwSetMouseButtonCallback(m_Window.Handle, &MousePressCallback);
    glfwSetFramebufferSizeCallback(m_Window.Handle, &FramebufferResizeCallback);
}

void Instance::Events(bool render) {
    for (size_t i = 0; i < Graphs[CurrentRenderGraph].Nodes.size( ); ++i) {
        Graphs[CurrentRenderGraph].Nodes[i].to_render = render;
    }
    if (glfwGetMouseButton(m_Window.Handle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS &&
        Graphs[CurrentRenderGraph].Cam.Type == CameraType::_3D) {
        double cMouseX, cMouseY;
        glfwGetCursorPos(m_Window.Handle, &cMouseX, &cMouseY);

        if (m_Window.MouseX != cMouseX || m_Window.MouseY != cMouseY) {
            float x = cMouseX - m_Window.MouseX;
            // float y = cMouseY - m_Window.MouseY;

            // Graphs[CurrentRenderGraph].PushCamera.Model = glm::rotate(
            //     Graphs[CurrentRenderGraph].PushCamera.Model, glm::radians(y * 0.25f),
            //     glm::cross(Graphs[CurrentRenderGraph].Cam.Position, glm::vec3(1.f, 0.f, 0.f)));
            Graphs[CurrentRenderGraph].PushCamera.Model = glm::rotate(
                Graphs[CurrentRenderGraph].PushCamera.Model, glm::radians(x * 0.25f), glm::vec3(0.f, 1.0f, 0.f));
            m_Window.MouseX = cMouseX;
            m_Window.MouseY = cMouseY;
        }
    }
}

void Instance::UpdateImGuiButton( ) {
    ImGuiIO &io = ImGui::GetIO( );

    for (int i = 0; i < 5; ++i) {
        io.MouseDown[i] = PressedButton[i] || glfwGetMouseButton(m_Window.Handle, i) != 0;
        PressedButton[i] = false;
    }

    const ImVec2 mouse_pos_backup = io.MousePos;
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    const bool focused = true;

    if (focused) {
        if (io.WantSetMousePos) {
            glfwSetCursorPos(m_Window.Handle, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);
        } else {
            double mouse_x, mouse_y;
            glfwGetCursorPos(m_Window.Handle, &mouse_x, &mouse_y);
            io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
        }
    }
}

}    // namespace Vulkan
}    // namespace ffGraph