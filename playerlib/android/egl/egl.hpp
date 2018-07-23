/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  egl.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/06
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  egl搭建opengles工作环境
*
*/
#ifndef _OPENGL_EGL_HPP_
#define _OPENGL_EGL_HPP_
extern "C" {
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
}
namespace render {
    class Egl {
    public:
        Egl();
        ~Egl();
    public:
        //
        // 初始化
        // 反初始化
        //
        bool initialize(EGLNativeWindowType window);
        void terminate();
        //
        //  设置当前的绘制目标
        //
        bool makeCurrent(EGLNativeWindowType window);
        //
        //提交后台缓冲区，开始显示
        //
        void swapBuffers();
        //
        //  是否是有效的状态，可以绘制
        //
        bool isValid();
    protected:
        //
        // 当前屏幕
        //
        EGLDisplay mDisplay;
        //
        //opengl工作的上下文
        //
        EGLContext mContext;
        //
        // anativewindow,opengl绘制的目标窗口
        // ANativeWindow
        //
        EGLNativeWindowType mWindow;
        //
        //  window对应创建的绘制表面，实现了opengl的协议接口
        //  他和mWindow是一样的东西
        //
        EGLSurface mSurface;
    public:
        //
        //  窗口大小
        //
        EGLint mWidth;
        EGLint mHeight;
    };

}
#endif