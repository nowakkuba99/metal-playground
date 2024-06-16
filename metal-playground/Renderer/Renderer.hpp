//
//  Renderer.hpp
//  metal-playground
//
//  Created by Jakub Nowak on 16/06/2024.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include <simd/simd.h>

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>


class Renderer {
public:
    Renderer(MTL::Device* device) : p_device(device->retain()) {
        p_commandQueue = p_device->newCommandQueue();
        buildShaders();
        buildBuffers();
    }
    ~Renderer() {
        p_device->release();
        p_commandQueue->release();
        p_PSO->release();
        p_vertexPositionsBuffer->release();
        p_fragmentColorsBuffer->release();
    }
    /**
     Builds a MTL::RenderPipelineState object from a pair of vertex and fragment shaders
     @note shaders can be specified in .metal files and complied separately!
     */
    void buildShaders();
    void buildBuffers();
    
    void draw(MTK::View* view);
private:
    MTL::Device* p_device;
    MTL::CommandQueue* p_commandQueue;
    MTL::RenderPipelineState* p_PSO;   // pipeline state object
    // buffers are unstructred memort allocations accessible by the GPU
    // app can use those without any constraints
    // here two buffers -> one for vertex data (3d positions of edges)
    // second for colors for each vertex
    MTL::Buffer* p_vertexPositionsBuffer;
    MTL::Buffer* p_fragmentColorsBuffer;
};

#endif /* Renderer_hpp */
