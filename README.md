

TODO:
Add suopport for multiple pipelines and descriptor sets
make the pipelines take in a reference to the descriptro sets that they want to exist on them 
remove vertex coloring
fix pipeline abstraction because multiple pipelines is a reasonable thing :)
completly re-do the descriptor pool / descriptor set abstraction since it's terribly segemnted right now

const std::array<vk::DescriptorPoolSize, 2> pools
        {
            vk::DescriptorPoolSize
            {
                .type            {vk::DescriptorType::eUniformBuffer},
                .descriptorCount {static_cast<std::uint32_t>(this->number_of_sets)}
            },
            vk::DescriptorPoolSize
            {
                .type            {vk::DescriptorType::eCombinedImageSampler},
                .descriptorCount {static_cast<std::uint32_t>(this->number_of_sets)}
            },
        };