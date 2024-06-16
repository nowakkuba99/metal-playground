//
//  myAppDelegate.hpp
//  metal-playground
//
//  Created by Jakub Nowak on 15/06/2024.
//

#ifndef myAppDelegate_hpp
#define myAppDelegate_hpp

#include <memory>

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

#include "../MTKViewDelegate/MTKViewDelegate.hpp"

class myAppDelegate : public NS::ApplicationDelegate {
public:
    virtual ~myAppDelegate() {
        p_window->release();
        p_mtkView->release();
        p_device->release();
    };
    
    NS::Menu* createMenuBar();
    
    virtual void applicationWillFinishLaunching( NS::Notification* pNotification ) override;
    virtual void applicationDidFinishLaunching( NS::Notification* pNotification ) override;
    virtual bool applicationShouldTerminateAfterLastWindowClosed( NS::Application* pSender) override {return true;};
private:
    NS::Window* p_window;   // OS Window
    MTK::View* p_mtkView;   // Display Metal content in the window
    MTL::Device* p_device;  // GPU abstraction object
    std::unique_ptr<myMTKViewDelegate> p_mtkViewDelegate;
};

#endif /* myAppDelegate_hpp */
