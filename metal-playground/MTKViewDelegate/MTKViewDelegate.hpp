//
//  MTKViewDelegate.hpp
//  metal-playground
//
//  Created by Jakub Nowak on 15/06/2024.
//

#ifndef MTKViewDelegate_hpp
#define MTKViewDelegate_hpp

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

#include "../Renderer/Renderer.hpp"


// Provides an interface where MTK::View can forward events
// By overriding functions from base class can respond to events
// Calls drawInMTKView every frame
class myMTKViewDelegate : public MTK::ViewDelegate {
public:
    myMTKViewDelegate(MTL::Device* device) : MTK::ViewDelegate() {
        p_renderer = std::make_unique<Renderer>(device);
    }
    virtual ~myMTKViewDelegate() = default;
    virtual void drawInMTKView(MTK::View* view) override;
private:
    std::unique_ptr<Renderer> p_renderer;
};

#endif /* MTKViewDelegate_hpp */
