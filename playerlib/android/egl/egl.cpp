/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  egl.cpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/06 
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  egl搭建opengles工作环境
*
*/
#include <FFL.h>
#include <android/native_window.h>
#include "egl.hpp"

namespace render {
    Egl::Egl() {
        mWidth = -1;
        mHeight = -1;

        mDisplay=EGL_NO_DISPLAY;
        mContext=EGL_NO_CONTEXT;
        mWindow=NULL;
        mSurface=EGL_NO_SURFACE;
    }

    Egl::~Egl() {
        terminate();
    }

    //
    // 初始化
    // 反初始化
    //
    bool Egl::initialize(EGLNativeWindowType window) {
        if (!window) {
            return EGL_FALSE;
        }

        EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            FFL_LOG_ERROR("[EGL] eglGetDisplay failed\n");
            return EGL_FALSE;
        }

        EGLint major, minor;
        if (!eglInitialize(display, &major, &minor)) {
            FFL_LOG_ERROR("[EGL] eglInitialize failed\n");
            return EGL_FALSE;
        }
        FFL_LOG_INFO("[EGL] eglInitialize %d.%d\n", (int) major, (int) minor);

        static const EGLint configAttribs[] = {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_NONE
        };
        static const EGLint contextAttribs[] = {
                EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL_NONE
        };

        EGLConfig config;
        EGLint numConfig;
        if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfig)) {
            FFL_LOG_ERROR("[EGL] eglChooseConfig failed\n");
            eglTerminate(display);
            return EGL_FALSE;
        }

#ifdef __ANDROID__
        {
            EGLint native_visual_id = 0;
            if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &native_visual_id)) {
                FFL_LOG_ERROR("[EGL] eglGetConfigAttrib() returned error %d", eglGetError());
                eglTerminate(display);
                return EGL_FALSE;
            }

            int32_t width = ANativeWindow_getWidth(window);
            int32_t height = ANativeWindow_getHeight(window);
            FFL_LOG_INFO("[EGL] ANativeWindow_setBuffersGeometry(f=%d);", native_visual_id);
            int ret = ANativeWindow_setBuffersGeometry(window, width, height, native_visual_id);
            if (ret) {
                FFL_LOG_ERROR("[EGL] ANativeWindow_setBuffersGeometry(format) returned error %d",
                              ret);
                eglTerminate(display);
                return EGL_FALSE;
            }
            mWidth = width;
            mHeight = height;
        }
#endif

        EGLSurface surface = eglCreateWindowSurface(display, config, window, NULL);
        if (surface == EGL_NO_SURFACE) {
            FFL_LOG_ERROR("[EGL] eglCreateWindowSurface failed\n");
            eglTerminate(display);
            return EGL_FALSE;
        }

        EGLSurface context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
        if (context == EGL_NO_CONTEXT) {
            FFL_LOG_ERROR("[EGL] eglCreateContext failed\n");
            eglDestroySurface(display, surface);
            eglTerminate(display);
            return EGL_FALSE;
        }

        if (!eglMakeCurrent(display, surface, surface, context)) {
            FFL_LOG_ERROR("[EGL] elgMakeCurrent() failed (new)\n");
            eglDestroyContext(display, context);
            eglDestroySurface(display, surface);
            eglTerminate(display);
            return EGL_FALSE;
        }

#if 0
#if defined(__ANDROID__)
        {
            const char *extensions = (const char *) glGetString(GL_EXTENSIONS);
            if (extensions) {
                char *dup_extensions = strdup(extensions);
                if (dup_extensions) {
                    char *brk = NULL;
                    char *ext = strtok_r(dup_extensions, " ", &brk);
                    while (ext) {
                        if (0 == strcmp(ext, "GL_EXT_texture_rg"))
                            egl->gles2_extensions[IJK_GLES2__GL_EXT_texture_rg] = 1;

                        ext = strtok_r(NULL, " ", &brk);
                    }

                    free(dup_extensions);
                }
            }
        }
#elif defined(__APPLE__)
        egl->gles2_extensions[IJK_GLES2__GL_EXT_texture_rg] = 1;
#endif
        FFL_INFO("[EGL] GLES2 extensions begin:\n");
        FFL_INFO("[EGL]     GL_EXT_texture_rg: %d\n", egl->gles2_extensions[IJK_GLES2__GL_EXT_texture_rg]);
        FFL_INFO("[EGL] GLES2 extensions end.\n");
#endif

        mContext = context;
        mSurface = surface;
        mDisplay = display;
        mWindow  =  window;
        return true;
    }

    void Egl::terminate() {
        if (!isValid()) {
            return;
        }

        eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (mContext) {
            eglDestroyContext(mDisplay, mContext);
            mContext = EGL_NO_CONTEXT;
        }
        if (mSurface) {
            eglDestroySurface(mDisplay, mSurface);
            mSurface = EGL_NO_SURFACE;
        }
        eglTerminate(mDisplay);
        mDisplay = EGL_NO_DISPLAY;
        mWindow=NULL;
        eglReleaseThread();
    }

//
//  设置当前的绘制目标
//
    bool Egl::makeCurrent(EGLNativeWindowType window) {
        if (window && window == mWindow && isValid()) {
            return eglMakeCurrent(mDisplay, mSurface, mSurface, mContext);
        }
        return initialize(window);
    }

//
//提交后台缓冲区，开始显示
//
    void Egl::swapBuffers() {
        eglSwapBuffers(mDisplay, mSurface);
        eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }

//
//  是否是有效的状态，可以绘制
//
    bool Egl::isValid() {
        return mDisplay && mWindow && mSurface;
    }
}