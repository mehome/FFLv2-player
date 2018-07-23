/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  RenderManager.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/28 
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  android下所有支持的渲染格式，方式的管理类，可以通过视频格式获取特定的渲染器
*
*/
#ifndef _RENDER_MANAGER_HPP_
#define _RENDER_MANAGER_HPP_

#include "VideoFormat.hpp"
#include "RenderInterface.hpp"
namespace android{
    //
    //  管理着支持的所有render
    //
    class RenderManager {
    public:
        RenderManager();
        ~RenderManager();
    public:
        //
        //  注册,反注册支持的所有render
        //
        void registerRenders();
        void unRegisterRenders();
        //
        //   根据输入的格式，返回一个可以使用的render
        //    format :需要的render格式
        //    arg  :参数，通过这个参数，找到一个最配置的render
        //
        RenderInterface *getRender(const player::VideoFormat* format, void *arg);
    };
}
#endif