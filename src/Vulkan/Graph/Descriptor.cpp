#include "Logger.h"
#include "Descriptor.h"
#include "GlobalEnvironment.h"

namespace ffGraph {
namespace Vulkan {

static DescriptorWriteType GetWriteType(VkDescriptorType type)
{
    if (type > VK_DESCRIPTOR_TYPE_END_RANGE || type < VK_DESCRIPTOR_TYPE_BEGIN_RANGE)
        return DescriptorWriteType::WRITE_TYPE_UNKNOW;
    DescriptorWriteType a[VK_DESCRIPTOR_TYPE_RANGE_SIZE] = {
        DescriptorWriteType::WRITE_TYPE_IMAGE,
        DescriptorWriteType::WRITE_TYPE_IMAGE,
        DescriptorWriteType::WRITE_TYPE_IMAGE,
        DescriptorWriteType::WRITE_TYPE_IMAGE,
        DescriptorWriteType::WRITE_TYPE_TEXEL,
        DescriptorWriteType::WRITE_TYPE_TEXEL,
        DescriptorWriteType::WRITE_TYPE_UNIFORM_BUFFER,
        DescriptorWriteType::WRITE_TYPE_UNIFORM_BUFFER,
        DescriptorWriteType::WRITE_TYPE_UNIFORM_BUFFER,
        DescriptorWriteType::WRITE_TYPE_UNIFORM_BUFFER,
        DescriptorWriteType::WRITE_TYPE_UNIFORM_BUFFER
    };

    return a[(size_t)type];
}

bool ConstructDescriptorPool(VkDescriptorPool& Pool, DescriptorList& DescriptorInfos)
{
    if (Pool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(GetLogicalDevice( ), Pool, 0);
        Pool = VK_NULL_HANDLE;
    }

    std::vector<VkDescriptorPoolSize> PoolSize = {};

    PoolSize.reserve(DescriptorInfos.size());

    for (size_t i = 0; i < DescriptorInfos.size(); ++i) {
        PoolSize[i].descriptorCount = DescriptorInfos[i].Count;
        PoolSize[i].type = DescriptorInfos[i].Type;
    }

    VkDescriptorPoolCreateInfo CreateInfos = {};
    CreateInfos.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    CreateInfos.poolSizeCount = PoolSize.size();
    CreateInfos.pPoolSizes = PoolSize.data();
    CreateInfos.maxSets = 1;

    if (vkCreateDescriptorPool(GetLogicalDevice( ), &CreateInfos, 0, &Pool))
        return false;
    return true;
}

bool ConstructDescriptorSets(VkDescriptorPool& Pool, VkDescriptorSetLayout& Layout, VkDescriptorSet& Set, DescriptorList& DescriptorInfos)
{
    if (Layout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(GetLogicalDevice( ), Layout, 0);
        Layout = VK_NULL_HANDLE;
    }
    std::vector<VkDescriptorSetLayoutBinding> Bindings = {};
    Bindings.reserve(DescriptorInfos.size());

    for (size_t i = 0; i < DescriptorInfos.size(); ++i) {
        Bindings[i].binding = i;
        Bindings[i].descriptorCount = DescriptorInfos[i].Count;
        Bindings[i].descriptorType = DescriptorInfos[i].Type;
        Bindings[i].stageFlags = DescriptorInfos[i].Stage;
    }

    VkDescriptorSetLayoutCreateInfo CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    CreateInfo.bindingCount = Bindings.size();
    CreateInfo.pBindings = Bindings.data();

    if (vkCreateDescriptorSetLayout(GetLogicalDevice( ), &CreateInfo, 0, &Layout))
        return false;

    VkDescriptorSetAllocateInfo AllocInfos = {};
    AllocInfos.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    AllocInfos.descriptorPool = Pool;
    AllocInfos.pSetLayouts = &Layout;
    AllocInfos.descriptorSetCount = 1;

    if (vkAllocateDescriptorSets(GetLogicalDevice(), &AllocInfos, &Set))
        return false;

    std::vector<VkWriteDescriptorSet> write(DescriptorInfos.size());

    for (size_t i = 0; i < DescriptorInfos.size(); ++i) {
        write[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write[i].dstSet = Set;
        write[i].descriptorType = DescriptorInfos[i].Type;
        write[i].descriptorCount = DescriptorInfos[i].Count;

        DescriptorWriteType WriteType = GetWriteType(DescriptorInfos[i].Type);
        if (WriteType == DescriptorWriteType::WRITE_TYPE_UNKNOW)
            return false;
        else if (WriteType == DescriptorWriteType::WRITE_TYPE_IMAGE) {
            write[i].pImageInfo = &DescriptorInfos[i].Data.ImageInfo;
        } else if (WriteType == DescriptorWriteType::WRITE_TYPE_UNIFORM_BUFFER) {
            write[i].pBufferInfo = &DescriptorInfos[i].Data.BufferInfo;
        } else {
            LogWarning("ConstructDescriptorSets", "We don't support this type of VkDescriptorType %d. \
                        The VkDescriptorSet isn't valid.\n", DescriptorInfos[i].Type);
        }
    }
    vkUpdateDescriptorSets(GetLogicalDevice( ), write.size(), write.data(), 0, 0);
}

} // namespace Vulkan
} // namespace ffGraph

