//
//  MTKViewDelegate.cpp
//  metal-playground
//
//  Created by Jakub Nowak on 15/06/2024.
//

#include "MTKViewDelegate.hpp"

void myMTKViewDelegate::drawInMTKView(MTK::View* view) {
    p_renderer->draw(view);
}

void Renderer::draw(MTK::View* view) {
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
    // Create command buffer - allows the app to encode commands for execution by the GPU
    MTL::CommandBuffer* cmd = p_commandQueue->commandBuffer();
    // Prepares the command buffer to receive drawing commands and spedicifes actions to perform when drawing starts and ends
    MTL::RenderPassDescriptor* rpd = view->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* enc = cmd->renderCommandEncoder(rpd);
    enc->endEncoding();
    // This encodes a command to make the results of the GPU work to the screen
    cmd->presentDrawable(view->currentDrawable());
    // Submits the encoded commands to the GPU for execution
    cmd->commit();
    
    pool->release();
}
