#ifndef DESCRIPTOR_H_
#define DESCRIPTOR_H_

#include <vulkan/vulkan.h>
#include <vector>

namespace ffGraph {
namespace Vulkan {

enum DescriptorWriteType {
    WRITE_TYPE_UNKNOW = -1,
    WRITE_TYPE_UNIFORM_BUFFER,
    WRITE_TYPE_IMAGE,
    WRITE_TYPE_TEXEL
};

struct ImageDescriptorInfo {
    VkImageView imageView;
    VkImageLayout imageLayout;
    VkSampler sampler;
};

struct BufferDescriptorInfo {
    VkBuffer buffer;
    VkDeviceSize offset;
    VkDeviceSize range;
};

struct TexelDescriptorInfo {
    VkBufferView bufferView;
    char padding[16];
};

struct Descriptor {
    VkDescriptorType Type;

    union {
        VkDescriptorImageInfo ImageInfo;
        VkDescriptorBufferInfo BufferInfo;
        VkBufferView BufferView;
    } Data;

    uint32_t Count = 1;
    VkShaderStageFlags Stage;
};

typedef std::vector<Descriptor> DescriptorList;

struct DescriptorHandle {
    uint16_t ffType;
    DescriptorList List;
};

bool ConstructDescriptorPool(VkDescriptorPool& Pool, DescriptorList& DescriptorInfos);
bool ConstructDescriptorSets(VkDescriptorPool& Pool, VkDescriptorSetLayout& Layout, VkDescriptorSet& Set, DescriptorList& DescriptorInfos);

} // namespace Vulkan
} // namespace ffGraph


#endif // DESCRIPTOR_H_