#include "Descriptors.h"
#include "GlobalEnvironment.h"

namespace ffGraph {
namespace Vulkan {

void PipelineDescriptor::Push(DescriptorInfo& n)
{
    DescriptorList.push_back(n);
    TotalNbOfDescriptor += n.Count;

    bool ToPush = true;
    for (size_t i = 0; i < UniqueTypes.size(); ++i) {
        if (n.Type == UniqueTypes[i].first) {
            UniqueTypes[i].second += 1;
            ToPush = false;
        }
    }
    if (ToPush)
        UniqueTypes.push_back({n.Type, 1});
}

bool PipelineDescriptor::init()
{
    if (DescriptorList.size() == 0)
        return false;

    std::vector<VkDescriptorPoolSize> PoolSize;
    PoolSize.resize(UniqueTypes.size());
    for (size_t i = 0; i < UniqueTypes.size(); ++i) {
        PoolSize[i].type = UniqueTypes[i].first;
        PoolSize[i].descriptorCount = UniqueTypes[i].second;
    }

    VkDescriptorPoolCreateInfo Infos = {};
    Infos.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    Infos.poolSizeCount = PoolSize.size();
    Infos.pPoolSizes = PoolSize.data();
    Infos.maxSets = TotalNbOfDescriptor;

    if (vkCreateDescriptorPool(GetLogicalDevice(), &Infos, 0, &DescriptorPool))
        return false;

    std::vector<VkDescriptorSetLayoutBinding> Bindings = {};
    Bindings.resize(DescriptorList.size());
    for (size_t i = 0; i < DescriptorList.size(); ++i) {
        Bindings[i].binding = i;
        Bindings[i].descriptorType = DescriptorList[i].Type;
        Bindings[i].stageFlags = DescriptorList[i].Stage;
        Bindings[i].descriptorCount = DescriptorList[i].Count;
    }

    VkDescriptorSetLayoutCreateInfo CreateInfos = {};
    CreateInfos.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    CreateInfos.bindingCount = Bindings.size();
    CreateInfos.pBindings = Bindings.data();

    if (vkCreateDescriptorSetLayout(GetLogicalDevice(), &CreateInfos, 0, &DescriptorLayout))
        return false;

    VkDescriptorSetAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    AllocInfo.descriptorPool = DescriptorPool;
    AllocInfo.descriptorSetCount = 1;
    AllocInfo.pSetLayouts = &DescriptorLayout;

    if (vkAllocateDescriptorSets(GetLogicalDevice(), 0, &DescriptorSet))
        return false;

    update();
    return true;
}

void PipelineDescriptor::update()
{
    std::vector<VkWriteDescriptorSet> WriteDesc = {};
    WriteDesc.resize(DescriptorList.size());

    for (size_t i = 0; i < DescriptorList.size(); ++i) {
        WriteDesc[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        WriteDesc[i].descriptorCount = DescriptorList[i].Count;
        WriteDesc[i].descriptorType = DescriptorList[i].Type;
        WriteDesc[i].dstSet = DescriptorSet;
        if (DescriptorList[i].DataType == FF_DESCRIPTOR_UNIFORM_BUFFER) {
            WriteDesc[i].pBufferInfo = (VkDescriptorBufferInfo *)DescriptorList[i].pNext;
        } else if (DescriptorList[i].DataType == FF_DESCRIPTOR_SAMPLER) {
            WriteDesc[i].pImageInfo = (VkDescriptorImageInfo *)DescriptorList[i].pNext;
        }
        vkUpdateDescriptorSets(GetLogicalDevice(), 1, &WriteDesc[i], 0, 0);
    }
}

DescriptorInfo NewDescriptor(VkDescriptorType Type, VkShaderStageFlags Stage, uint8_t DataType, void *pData)
{
    DescriptorInfo n = {};

    n.Type = Type;
    n.Stage = Stage;
    n.DataType = DataType;
    n.pNext = pData;
    return n;
}

}
}