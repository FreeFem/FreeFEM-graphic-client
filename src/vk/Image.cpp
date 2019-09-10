#include "Image.h"

namespace FEM {
namespace VK {

bool newImage(const ImageFactory ImgFactory, Image *Img, bool CreateWithView, const ImageInfos Infos) {
    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = 0;
    imageCreateInfo.flags = 0;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = Infos.Format;
    imageCreateInfo.extent = {Infos.Width, Infos.Height, 1};
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = Infos.Usage;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = 0;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    Img->ImgInfos = Infos;
    if (vmaCreateImage(*ImgFactory.AllocatorREF, &imageCreateInfo, &Infos.AllocInfos, &Img->Handle, &Img->Memory,
                       &Img->MemoryInfos))
        return false;
    if (CreateWithView) {
        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = 0;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = Img->Handle;
        imageViewCreateInfo.format = Infos.Format;
        imageViewCreateInfo.subresourceRange.aspectMask = Infos.AspectMask;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

        if (vkCreateImageView(*ImgFactory.DeviceREF, &imageViewCreateInfo, 0, &Img->View)) return false;
    } else {
        Img->View = VK_NULL_HANDLE;
    }
    return true;
}

void destroyImage(const ImageFactory ImgFactory, Image Img) {
    vkDestroyImageView(*ImgFactory.DeviceREF, Img.View, 0);
    vmaDestroyImage(*ImgFactory.AllocatorREF, Img.Handle, Img.Memory);
}

}    // namespace VK
}    // namespace FEM