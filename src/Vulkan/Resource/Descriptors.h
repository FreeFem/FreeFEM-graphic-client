#ifndef DESCRIPTORS_H_
#define DESCRIPTORS_H_

#include <vulkan/vulkan.h>
#include <vector>

namespace ffGraph {
namespace Vulkan {

enum DescriptorInfoType {
    FF_DESCRIPTOR_UNIFORM_BUFFER,
    FF_DESCRIPTOR_SAMPLER
};

using DescriptorBufferInfo = VkDescriptorBufferInfo;
using DescriptorImageInfo = VkDescriptorImageInfo;

struct SamplerInfo {
    uint8_t sType = FF_DESCRIPTOR_SAMPLER;
    VkSampler Sampler;
    VkImageView View;
};

struct DescriptorInfo {
    VkDescriptorType Type;
    VkShaderStageFlags Stage;
    uint8_t Count;

    uint8_t DataType;
    void *pNext;
};

class PipelineDescriptor {
    public:
        std::vector<DescriptorInfo> DescriptorList;
        std::vector<std::pair<VkDescriptorType, uint8_t>> UniqueTypes;
        uint32_t TotalNbOfDescriptor = 0;

        inline DescriptorInfo& operator[](size_t i) { return DescriptorList[i]; }
        inline size_t size() { return DescriptorList.size(); }

        void Push(DescriptorInfo& n);
        bool init();
        void update();

        VkDescriptorPool DescriptorPool;
        VkDescriptorSetLayout DescriptorLayout;
        VkDescriptorSet DescriptorSet;

};

DescriptorInfo NewDescriptor(VkDescriptorType Type, VkShaderStageFlags Stage, void *pData);

VkDescriptorPool CreateDescriptorPool(PipelineDescriptor& List);

} // namespace Vulkan
} // namespace ffGraph


#endif // DESCRIPTORS_H_