//
//  myAppDelegate.cpp
//  metal-playground
//
//  Created by Jakub Nowak on 15/06/2024.
//

#include "myAppDelegate.hpp"

void myAppDelegate::applicationWillFinishLaunching( NS::Notification* pNotification ) {
    NS::Menu* menu = createMenuBar();
    // Get application from notification and activate
    NS::Application* p_app = reinterpret_cast<NS::Application*>(pNotification->object());
    p_app->setMainMenu(menu);
    p_app->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);
};
void myAppDelegate::applicationDidFinishLaunching( NS::Notification* pNotification ) {
    // Create frame object
    CGRect frame {CGRectMake(100.0, 100.0, 512.0, 512.0)};
    // Create window
    p_window = NS::Window::alloc()->init(frame, NS::WindowStyleMaskClosable|NS::WindowStyleMaskTitled, NS::BackingStoreBuffered, false);
    // Create Device
    p_device = MTL::CreateSystemDefaultDevice();
    // Create View
    p_mtkView = MTK::View::alloc()->init(frame, p_device);
    p_mtkView->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    // Set background color
    p_mtkView->setClearColor(MTL::ClearColor::Make(107.0/255.0, 107.0/255.0, 107.0/255.0, 0.0));
    
    // Create custom mtkViewDelegate
    p_mtkViewDelegate = std::make_unique<myMTKViewDelegate>(p_device);
    p_mtkView->setDelegate(p_mtkViewDelegate.get());
    
    p_window->setContentView(p_mtkView);
    // Set title
    p_window->setTitle(NS::String::string("metal-playground",NS::StringEncoding::UTF8StringEncoding));
    // ?
    p_window->makeKeyAndOrderFront(nullptr);
    
    NS::Application* app {reinterpret_cast<NS::Application*>(pNotification->object())};
    app->activateIgnoringOtherApps(true);
};

NS::Menu *myAppDelegate::createMenuBar() { 
    using NS::StringEncoding::UTF8StringEncoding;
    
    NS::Menu* mainMenu = NS::Menu::alloc()->init();
    NS::MenuItem* appMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu* appMenu = NS::Menu::alloc()->init(NS::String::string("Appname",UTF8StringEncoding));
    
    NS::String* appName = NS::RunningApplication::currentApplication()->localizedName();
    NS::String* quitItemName = NS::String::string("Quit ",UTF8StringEncoding)->stringByAppendingString(appName);
    SEL quitCb = NS::MenuItem::registerActionCallback("appQuit", [](void*,SEL,const NS::Object* sender){
        auto app = NS::Application::sharedApplication();
        app->terminate(sender);
    });
    
    NS::MenuItem* appQuitItem = appMenu->addItem(quitItemName, quitCb, NS::String::string("q",UTF8StringEncoding));
    appQuitItem->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);
    appMenuItem->setSubmenu(appMenu);
    
    
    NS::MenuItem* windowMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu* windowMenu = NS::Menu::alloc()->init(NS::String::string("Window Menu",UTF8StringEncoding));
    
    SEL closeWindowCb = NS::MenuItem::registerActionCallback("windowClose", [](void*, SEL, const NS::Object*){
        auto app = NS::Application::sharedApplication();
        app->windows()->object<NS::Window>(0)->close();
    });
    NS::MenuItem* closeWindowItem = windowMenu->addItem(NS::String::string("Close app window",UTF8StringEncoding), closeWindowCb, NS::String::string("w",UTF8StringEncoding));
    windowMenuItem->setSubmenu(windowMenu);
    closeWindowItem->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);
    
    mainMenu->addItem(appMenuItem);
    mainMenu->addItem(windowMenuItem);
    
    appMenuItem->release();
    windowMenuItem->release();
    appMenu->release();
    windowMenu->release();
    
    return mainMenu->autorelease();
}

