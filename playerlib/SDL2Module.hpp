/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  SDL2Module.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/28
*
*  sdl2模块的相关封装
*
*/

#ifndef _SDL2_MODULE_HPP
#define _SDL2_MODULE_HPP

#include <SDL.h>
#include "SDL2VideoDevice.hpp"
#include "SDL2AudioDevice.hpp"
#include "DeviceFactory.hpp"


namespace player {
	class SDL2VideoDevice;
	class SDL2AudioDevice;
	
	class SDL2Module : public DeviceFactory {
	public:
		SDL2Module();
		~SDL2Module();	
		//
		//  创建播放视频设备
		//
		virtual FFL::sp<VideoDevice> createVideoDevice(void* player) ;
		//
		//  创建播放音频设备
		//
		virtual FFL::sp<AudioDevice> createAudioDevice(void* player) ;
	};

	//返回false就会退出循环
	typedef bool (*FUNKeyPress)(void* userdata,int key);
	void SDL2Loop(FUNKeyPress fun, void* userdata, bool* quit);	
}

#endif