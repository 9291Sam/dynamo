

TODO:
Add suopport for multiple pipelines and descriptor sets
completly re-do the descriptor pool / descriptor set abstraction since it's terribly segemnted right now


Pipeline Refactor:
    Renderer::createObject(std::vector<Vertex>, std::vector<Index>, enum Pipelines)


    At start of renderer:
        create all pipelines in a big vector
        pass references to this to the objects that need them.


```cpp
enum Pipelines
{
    Voxel,
    HUD,
    Particle,
    NoPipelineBound,
}
```

actual drawing
```cpp
std::vector<std::pair<const Pipeline&, std::vector<Object>>> objectsWithPipeline;

for (const auto [p, v_o] : objectsWithPipeline)
{
    commandBuffer.bindPipeline(*p);

    for (const Object& o : v_o)
    {
        o.bind();
        o.draw();
    }
}


Pipelines currentPipeline;