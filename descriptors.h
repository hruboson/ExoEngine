#pragma once

#include "device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace exo {

    // Descriptors are kind of pointers to resources (e.g.buffer, texture)

    class ExoDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(ExoDevice& exoDevice) : device{ exoDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<ExoDescriptorSetLayout> build() const;

        private:
            ExoDevice& device;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        ExoDescriptorSetLayout(
            ExoDevice& exoDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~ExoDescriptorSetLayout();
        ExoDescriptorSetLayout(const ExoDescriptorSetLayout&) = delete;
        ExoDescriptorSetLayout& operator=(const ExoDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        ExoDevice& device;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class ExoDescriptorWriter; // friend class can access to private members of this class
    };

    class ExoDescriptorPool {
    public:
        class Builder {
        public:
            Builder(ExoDevice& exoDevice) : device{ exoDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count); // e.g. 100 uniform buffers and 100 image samplers
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags); // config behaviour of the pool
            Builder& setMaxSets(uint32_t count); // total number of descriptor sets
            std::unique_ptr<ExoDescriptorPool> build() const;

        private:
            ExoDevice& device;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        ExoDescriptorPool(
            ExoDevice& device,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~ExoDescriptorPool();
        ExoDescriptorPool(const ExoDescriptorPool&) = delete;
        ExoDescriptorPool& operator=(const ExoDescriptorPool&) = delete;

        bool allocateDescriptorSet(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        ExoDevice& device;
        VkDescriptorPool descriptorPool;

        friend class ExoDescriptorWriter; // friend class can access to private members of this class
    };

    class ExoDescriptorWriter { // allocates vkDescriptorSet from the pool and write the necessary information to each descriptor
    public:
        ExoDescriptorWriter(ExoDescriptorSetLayout& setLayout, ExoDescriptorPool& pool);

        ExoDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        ExoDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        ExoDescriptorSetLayout& setLayout;
        ExoDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };
}