/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  DeviceManager.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/01 
*  https://github.com/zhenfei2016/FFL-v2.git
*  音视频设备管理，设备的打开，关闭，获取已经缓存的音视频设备
*
*/
#ifndef _DEVICE_MANAGER_HPP_
#define _DEVICE_MANAGER_HPP_

#include "VideoDevice.hpp"
#include "AudioDevice.hpp"
#include "VideoStream.hpp"
#include "AudioStream.hpp"

namespace player {
	class DeviceManager {
	public:
		DeviceManager();
		virtual ~DeviceManager();

		//
		//  通过wnd创建对应的
		//
		virtual FFL::sp<VideoDevice> openVideoDisplay(SurfaceHandle wnd) = 0;
		//
		//  如果式通过已经创建的SufraceHandle 创建的VideoDevice则不会关闭这个窗口的
		//
		virtual void closeVideoDisplay() = 0;
		//
		//  通过fmt打开对应的音频设备
		//
		virtual FFL::sp<AudioDevice> openAudioDisplay(AudioFormat fmt) = 0;
		//
		//  关闭音频设备
		//
		virtual void closeAudioDisplay() = 0;
		//
		//  获取已经创建的视频设备
		//
		virtual FFL::sp<VideoDevice> getVideoDisplay(FFL::sp<VideoStream> stream) = 0;
		//
		// 获取已经创建的音频设备
		//
		virtual FFL::sp<AudioDevice> getAudioDisplay(FFL::sp<AudioStream> stream) = 0;

    public:
        SurfaceHandle mSurfaceHandle;
	};
}
#endif
