#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <vulkan/vulkan.h>
#include "../util/utils.h"
#include "../util/NonCopyable.h"

class CommandBufferCreator : public NonCopyable {
    public:
        CommandBufferCreator() = default;

        ~CommandBufferCreator() = default;

        FORCE_USE_RESULT Error init(VkDevice& device, uint32_t queueFamily);

        void destroy();

        FORCE_USE_RESULT Error newCommandBuffer(const VkCommandBufferLevel commandBufferLevel, VkCommandBuffer& commandBuffer);

    private:
        static constexpr VkCommandPoolCreateFlagBits flagsBits = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VkCommandPool m_commandPool = VK_NULL_HANDLE;
        VkDevice m_device = VK_NULL_HANDLE;
        uint32_t m_queueFamily;
};

#endif // COMMAND_BUFFER_H