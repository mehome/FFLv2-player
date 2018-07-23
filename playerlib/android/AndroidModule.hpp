/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  AndroidModule.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/06/28
*  https://github.com/zhenfei2016/FFL-v2.git
*  android播放输出模块定义
*
*/
#ifndef _ANDROID_MODULE_HPP
#define _ANDROID_MODULE_HPP

#include "DeviceFactory.hpp"
#include "AndroidAudioDevice.hpp"
#include "AndroidVideoDevice.hpp"

namespace android {
	class AndroidModule : public player::DeviceFactory {
	public:
		AndroidModule();
		~AndroidModule();
		//
		//  创建播放视频设备
		//
		virtual FFL::sp<player::VideoDevice> createVideoDevice(void* player) ;
		//
		//  创建播放音频设备
		//
		virtual FFL::sp<player::AudioDevice> createAudioDevice(void* player) ;
	};
}

#endif