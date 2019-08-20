#include "CommandBuffer.h"

Error CommandBufferCreator::init(VkDevice& device, uint32_t queueFamily)
{
    m_device = device;
    m_queueFamily = queueFamily;

    VkResult result;

    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = flagsBits;
    createInfo.queueFamilyIndex = m_queueFamily;

    result = vkCreateCommandPool(m_device, &createInfo, 0, &m_commandPool);
    if (result != VK_SUCCESS)
        return Error::FUNCTION_FAILED;
    return Error::NONE;
}

void CommandBufferCreator::destroy()
{
    vkDestroyCommandPool(m_device, m_commandPool, 0);
}

Error CommandBufferCreator::newCommandBuffer(const VkCommandBufferLevel commandBufferLevel, VkCommandBuffer& commandBuffer)
{
    VkResult result;

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = commandBufferLevel;
    allocInfo.commandBufferCount = 1;

    result = vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);
    if (result != VK_SUCCESS)
        return Error::FUNCTION_FAILED;
    return Error::NONE;
}