#ifndef IMAGE_H_
#define IMAGE_H_

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

namespace FEM {
namespace VK {

struct ImageInfos {
    uint32_t Width, Height;
    VkFormat Format;
    VkBufferUsageFlags Usage;
    VkImageAspectFlags AspectMask;
    VmaAllocationCreateInfo AllocInfos;
};

struct ImageFactory {
    VkDevice *DeviceREF;
    VmaAllocator *AllocatorREF;
};

struct Image {
    VkImage Handle;
    VkImageView View;
    VmaAllocation Memory;
    VmaAllocationInfo MemoryInfos;
    ImageInfos ImgInfos;
};

bool newImage(const ImageFactory ImgFactory, Image *Img, bool CreateWithView, const ImageInfos Infos);

void destroyImage(const ImageFactory ImgFactory, Image Img);

}
}

#endif // IMAGE_H_