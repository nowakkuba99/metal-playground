//
//  Renderer.cpp
//  metal-playground
//
//  Created by Jakub Nowak on 16/06/2024.
//

#include "Renderer.hpp"

void Renderer::draw(MTK::View* view) {
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
    // Create command buffer - allows the app to encode commands for execution by the GPU
    MTL::CommandBuffer* cmd = p_commandQueue->commandBuffer();
    // Prepares the command buffer to receive drawing commands and spedicifes actions to perform when drawing starts and ends
    MTL::RenderPassDescriptor* rpd = view->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* enc = cmd->renderCommandEncoder(rpd);
    
    // Add commands to draw triangle from buffer
    enc->setRenderPipelineState(p_PSO);
//    enc->setVertexBuffer(p_vertexPositionsBuffer, 0, 0);
//    enc->setVertexBuffer(p_fragmentColorsBuffer, 0, 1);
    // Use argument buffer
    enc->setVertexBuffer(p_argumentBuffer, 0, 0);
    enc->useResource(p_vertexPositionsBuffer, MTL::ResourceUsageRead);
    enc->useResource(p_fragmentColorsBuffer, MTL::ResourceUsageRead);
    enc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
    
    enc->endEncoding();
    // This encodes a command to make the results of the GPU work to the screen
    cmd->presentDrawable(view->currentDrawable());
    // Submits the encoded commands to the GPU for execution
    cmd->commit();
    
    pool->release();
}

void Renderer::buildShaders() {
    using NS::StringEncoding::UTF8StringEncoding;
    // Shaders source code
    std::string shaderSrc = R"(
        #include <metal_stdlib>
        using namespace metal;

        struct v2f
        {
            float4 position [[position]];
            half3 color;
        };

        struct VertexData
        {
            device float3* positions [[id(0)]];
            device float3* colors [[id(1)]];
        };

        v2f vertex vertexMain( device const VertexData* vertexData [[buffer(0)]], uint vertexId [[vertex_id]] )
        {
            v2f o;
            o.position = float4( vertexData->positions[ vertexId ], 1.0 );
            o.color = half3(vertexData->colors[ vertexId ]);
            return o;
        }

        half4 fragment fragmentMain( v2f in [[stage_in]] )
        {
            return half4( in.color, 1.0 );
        }
    )";
    // This build intermediate represetation of shader code
    NS::Error* error {nullptr};
    MTL::Library* library = p_device->newLibrary(NS::String::string(shaderSrc.c_str(),UTF8StringEncoding), nullptr, &error);
    if(!library) {
        __builtin_printf( "%s", error->localizedDescription()->utf8String() );
        assert( false );
    }
    // Obtain the functions from library
    MTL::Function* vertexFn = library->newFunction(NS::String::string("vertexMain",UTF8StringEncoding));
    MTL::Function* fragmentFn = library->newFunction(NS::String::string("fragmentMain",UTF8StringEncoding));
    
    // Designate the two shaders the pipeline should use, also specify he pixel format from MTK::View
    MTL::RenderPipelineDescriptor* desc = MTL::RenderPipelineDescriptor::alloc()->init();
    desc->setVertexFunction(vertexFn);
    desc->setFragmentFunction(fragmentFn);
    desc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    
    // Create concrete pipelineState object via newRenderPipelineState
    // RenderPipelineState object are expensive to create since there is shader compilation involved
    // Do it once e.g. when app starts
    p_PSO = p_device->newRenderPipelineState(desc, &error);
    if(!p_PSO) {
        __builtin_printf( "%s", error->localizedDescription()->utf8String() );
        assert( false );
    }
    
    vertexFn->release();
    fragmentFn->release();
    desc->release();
//    library->release();
    p_shaderLibrary = library;
}

void Renderer::buildBuffers() {
    const size_t numVertices {3};
    
    // Data to allocate
    simd::float3 positions[numVertices] =
    {
        { -0.8f,  0.8f, 0.0f },
        {  0.0f, -0.8f, 0.0f },
        { +0.8f,  0.8f, 0.0f }
    };
    
    simd::float3 colors[numVertices] =
    {
        {  1.0, 0.3f, 0.2f },
        {  0.8f, 1.0, 0.0f },
        {  0.8f, 0.0f, 1.0 }
    };
    
    // Calculate size
    const size_t positionDataSize = numVertices * sizeof(simd::float3);
    const size_t colorDataSize = numVertices * sizeof(simd::float3);
    
    // Create buffers
    //MTL::ResourceStorageModeManaged indicates that both CPU and GPU can directly access the contents of the buffer
    p_vertexPositionsBuffer = p_device->newBuffer(positionDataSize, MTL::ResourceStorageModeManaged);
    p_fragmentColorsBuffer = p_device->newBuffer(colorDataSize, MTL::ResourceStorageModeManaged);
    
    // Copy data to buffers
    memcpy(p_vertexPositionsBuffer->contents(), positions, positionDataSize);
    memcpy(p_fragmentColorsBuffer->contents(), colors, colorDataSize);
    
    // Incicate to Metal that there is data in the buffers
    p_vertexPositionsBuffer->didModifyRange(NS::Range::Make(0, p_vertexPositionsBuffer->length()));
    p_fragmentColorsBuffer->didModifyRange(NS::Range::Make(0, p_fragmentColorsBuffer->length()));
    
    using NS::StringEncoding::UTF8StringEncoding;
    // Add argument buffer
    assert(p_shaderLibrary);
    MTL::Function* vertexFn = p_shaderLibrary->newFunction(NS::String::string("vertexMain",UTF8StringEncoding));
    MTL::ArgumentEncoder* argumentEnc = vertexFn->newArgumentEncoder(0);
    MTL::Buffer* argBuffer = p_device->newBuffer( argumentEnc->encodedLength(), MTL::ResourceStorageModeManaged );
    p_argumentBuffer = argBuffer;

    argumentEnc->setArgumentBuffer( argBuffer, 0 );

    argumentEnc->setBuffer( p_vertexPositionsBuffer, 0, 0 );
    argumentEnc->setBuffer( p_fragmentColorsBuffer, 0, 1 );

    p_argumentBuffer->didModifyRange( NS::Range::Make( 0, p_argumentBuffer->length() ) );

    vertexFn->release();
    argumentEnc->release();
}
