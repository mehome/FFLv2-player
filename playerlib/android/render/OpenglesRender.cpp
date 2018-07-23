/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  OpenglesRender.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/06
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  opengl方式绘制图片
*/
#include <VideoTexture.hpp>
#include "OpenglesRender.hpp"
#include "VideoSurface.hpp"
#include "android/opengles2/SDL_render_gles2.hpp"
#include "android/egl/egl.hpp"

namespace  render{
    OpenglEsRender::OpenglEsRender(){
        mEgl=new Egl();
        mRenderer=NULL;
    }
    OpenglEsRender::~OpenglEsRender(){
        if(mEgl){
            mEgl->terminate();
        }
        FFL_SafeFree(mEgl);
        mEgl=NULL;
    }

    //  获取支持的格式
    //  wanted: 如果为nUll则返回所有支持的格式
    //           非null 返回跟他匹配的
    //  fmtList: 返回支持的格式list
    //
    void OpenglEsRender::getSupportFormat(player::VideoSurface* surface,const player::VideoFormat* wanted,FFL::List<player::VideoFormat>& fmtList){
        player::VideoFormat targetFormat;
        surface->getFormat(targetFormat);
        fmtList.push_back(targetFormat);
    }
    bool OpenglEsRender::getOptimalFormat(player::VideoSurface* surface,const player::VideoFormat* wanted,player::VideoFormat* optinal){
        player::VideoFormat targetFormat;
        surface->getFormat(targetFormat);
        if(optinal){
            *optinal=targetFormat;
        }
        return true;
    }
    bool OpenglEsRender::isSupportFormat(player::VideoSurface* surface,const player::VideoFormat* wanted) {
        return true;
    }
    //
    //  创建这个render ，arg自定义的参数
    status_t OpenglEsRender::create(player::VideoSurface* surface,const player::VideoFormat*  arg){
        if(mEgl->isValid()) {
            FFL_LOG_ERROR("Failed to OpenglEsRender::create egl valid");
            mEgl->terminate();
        }

        mEgl->initialize(surface->getHandle());
        if(mRenderer==NULL){
            mRenderer=new  GLES2Renderer();
            mRenderer->install(arg,mEgl->mWidth,mEgl->mHeight);
        }
        return FFL_OK;
    }
    //
    //  绘制图片tex到 surface上面
    //
    status_t OpenglEsRender::draw(player::VideoSurface* surface,player::VideoTexture* tex){
        if(!mEgl->isValid() || mRenderer ==NULL){
            FFL_LOG_ERROR("Failed to OpenglEsRender::draw egl invalid");
            return  FFL_FAILED;
        }

        mEgl->makeCurrent(surface->getHandle());
        mRenderer->useProgram();
        mRenderer->updateViewport(mEgl->mWidth,mEgl->mHeight);
        mRenderer->clearColor(1.0,0.0,1.0,1.0);
        SDL_Rect srcRect={0,0,tex->mWidth,tex->mHeight};
        SDL_FRect dstRect={-1.f,-1.0f,2.0f,2.0f};
        double angle=0;
        SDL_FPoint center={1.f,1.f};
        SDL_RendererFlip flip=SDL_FLIP_VERTICAL;

        mRenderer->uploadTexture(tex,&srcRect);
        mRenderer->render(tex,&srcRect,&dstRect,angle,&center,flip);
        mEgl->swapBuffers();
        return FFL_OK;
    }
    //
    //  释放这个render，释放后这个render就不可用了
    //
    void OpenglEsRender::destroy(){
        if(mRenderer!= NULL){
            mRenderer->uninstall();
            FFL_SafeFree(mRenderer);
            mRenderer=NULL;
        }

        if(mEgl){
            mEgl->terminate();
        }
    }
}