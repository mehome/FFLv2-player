/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  OpenglesRender.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/06 
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  opengl方式绘制图片
*/
#ifndef _OPENGLES_RENDER_HPP_
#define _OPENGLES_RENDER_HPP_
#include "RenderInterface.hpp"
#include "VideoFormat.hpp"
#include <FFL.h>

class GLES2Renderer;
namespace render{
    class Egl;
    class OpenglEsRender  : public android::RenderInterface{
    public:
        OpenglEsRender();
        ~OpenglEsRender();

        //  获取支持的格式
        //  wanted: 如果为nUll则返回所有支持的格式
        //           非null 返回跟他匹配的
        //  fmtList: 返回支持的格式list
        //
        virtual void getSupportFormat(player::VideoSurface* surface,const player::VideoFormat* wanted,FFL::List<player::VideoFormat>& fmtList);
        virtual bool getOptimalFormat(player::VideoSurface* surface,const player::VideoFormat* wanted,player::VideoFormat* optinal);
        virtual bool isSupportFormat(player::VideoSurface* surface,const player::VideoFormat* wanted);
        //
        //  创建这个render ，arg自定义的参数
        //
        virtual status_t create(player::VideoSurface* surface,const player::VideoFormat*  arg);
        //
        //  绘制图片tex到 surface上面
        //
        virtual status_t draw(player::VideoSurface* surface,player::VideoTexture* tex);
        //
        //  释放这个render，释放后这个render就不可用了
        //
        virtual void destroy();

    private:
        Egl* mEgl;
        GLES2Renderer* mRenderer;
    };
}
#endif
