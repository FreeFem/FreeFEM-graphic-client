#ifndef INSTANCE_H_
#define INSTANCE_H_

#include <vector>
#include <vulkan/vulkan.h>
#include <memory>
#include <deque>
#include "vk_mem_alloc.h"
#include "Window/NativeWindow.h"
#include "Context/Context.h"
#include "RenderGraph/GraphConstructor.h"
#include "Resource/Resource.h"
#include "Renderer/Renderer.h"

namespace ffGraph {
namespace Vulkan {

class Instance {
    public:
        NativeWindow m_Window;
        VkInstance m_Handle;
        std::vector<const char *> Extensions;

        Context vkContext;
        VmaAllocator Allocator;
        GraphConstructor GraphConstruct;
        Resource Resources;
        Renderer vkRenderer;

        uint32_t CurrentRenderGraph = 0;
        std::vector<RenderGraph> Graphs = {};


#ifdef _DEBUG
        VkDebugUtilsMessengerEXT m_DebugMessenger;
#endif
        Instance();
        Instance(NativeWindow& Window);

        // Deleting copy constructor
        Instance(Instance&) = delete;
        Instance& operator=(Instance&) = delete;

        void load(const std::string& AppName, unsigned int width, unsigned int height);
        void destroy();
        void run(std::shared_ptr<std::deque<std::string>> SharedQueue);
};

} // namespace Vulkan
} // namespace ffGraph


#endif // INSTANCE_H_