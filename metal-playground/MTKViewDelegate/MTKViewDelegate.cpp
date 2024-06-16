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
