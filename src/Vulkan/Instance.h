#ifndef INSTANCE_H_
#define INSTANCE_H_

#include <vector>
#include <vulkan/vulkan.h>
#include <memory>
#include <deque>
#include "vk_mem_alloc.h"
#include "Environment.h"
#include "Frame.h"
#include "Resource/Shader.h"
#include "ThreadQueue.h"
#include "ImGui_Impl.h"

namespace ffGraph {
namespace Vulkan {

class Instance {
   public:
    NativeWindow m_Window;
    VkInstance m_Handle;
    std::vector<const char*> Extensions;

    Environment Env;
    uint32_t CurrentFrameData = 0;
    PerFrame FrameData[2];
    ShaderLibrary Shaders;

    UiPipeline Ui;

    bool PressedButton[5] = {false, false, false, false, false};

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT m_DebugMessenger;
#endif
    Instance( );
    Instance(NativeWindow& Window);

    // Deleting copy constructor
    Instance(Instance&) = delete;
    Instance& operator=(Instance&) = delete;

    void load(const std::string& AppName, unsigned int width, unsigned int height);
    void reload( );
    void destroy( );
    void run(std::shared_ptr<std::deque<std::string>> SharedQueue, JSON::ThreadSafeQueue& GeometryQueue);
    void render( );
    void renderUI( );

    void UpdateImGuiButton( );

   private:
    void initGFLWCallbacks( );
    void Events(bool r);
};

bool pushInitCmdBuffer(const VkDevice& Device, const VkQueue& Queue, const Image DepthImage, const Image ColorImage,
                       const VkCommandPool& Pool);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // INSTANCE_H_