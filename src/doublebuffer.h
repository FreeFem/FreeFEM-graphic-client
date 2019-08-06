#ifndef DOUBLEBUFFER_H
#define DOUBLEBUFFER_H

#include <vulkan/vulkan.h>

struct perFrameData {
    VkCommandBuffer presentCmdBuffer;
    VkSemaphore imageAcquiredSemaphore;
    VkSemaphore renderingCompletedSemaphore;
    VkFence presentFence;
    bool fenceInitialized;
};

#endif // DOUBLEBUFFER_H