/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  AndroidModule.cpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/28 
*  https://github.com/zhenfei2016/FFL-v2.git
*  android播放输出模块定义
*
*/
#include "AndroidModule.hpp"

namespace android {
	AndroidModule::AndroidModule()  {

	}
	AndroidModule::~AndroidModule() {

	}
	//
	//  创建播放视频设备
	//
	FFL::sp<player::VideoDevice> AndroidModule::createVideoDevice(void* player) {
		return  new AndroidVideoDevice();
	}
	//
	//  创建播放音频设备
	//
	FFL::sp<player::AudioDevice> AndroidModule::createAudioDevice(void* player) {
		return new AndroidAudioDevice();
	}



}
