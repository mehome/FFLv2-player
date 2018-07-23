/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  DeviceManager.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/01
*  https://github.com/zhenfei2016/FFL-v2.git
*  音视频设备管理，设备的打开，关闭，获取已经缓存的音视频设备
*
*/
#include "DeviceManager.hpp"
#include "VideoDevice.hpp"
#include "AudioDevice.hpp"

namespace player {
	DeviceManager::DeviceManager(){
		mSurfaceHandle=NULL;
	}
	DeviceManager::~DeviceManager() {
	}
}