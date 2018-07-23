/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  DefaultDeviceManager.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/07/01 
*  https://github.com/zhenfei2016/FFL-v2.git
*  音视频设备管理，设备的打开，关闭，获取已经缓存的音视频设备
*  默认使用的，android平台下使用 AndroidModule.hpp  ，其他平台使用SDL2Module.hpp
*
*/
#ifndef _DEFAULT_DEVICE_MANAGER_HPP_
#define _DEFAULT_DEVICE_MANAGER_HPP_

#include "DeviceManager.hpp"
namespace player {
	class DeviceFactory;
	class FFLPlayer;

	class DefaultDeviceManager : public DeviceManager{
	public:
		DefaultDeviceManager();
		virtual ~DefaultDeviceManager();
		//
		//  通过wnd创建对应的
		//
		virtual FFL::sp<VideoDevice> openVideoDisplay(SurfaceHandle wnd) ;
		//
		//  如果式通过已经创建的SufraceHandle 创建的VideoDevice则不会关闭这个窗口的
		//
		virtual void closeVideoDisplay();
		//
		//  通过fmt打开对应的音频设备
		//
		virtual FFL::sp<AudioDevice> openAudioDisplay(AudioFormat fmt) ;
		//
		//  关闭音频设备
		//
		virtual void closeAudioDisplay() ;
		//
		//  获取已经创建的视频设备
		//
		virtual FFL::sp<VideoDevice> getVideoDisplay(FFL::sp<VideoStream> stream) ;
		//
		// 获取已经创建的音频设备
		//
		virtual FFL::sp<AudioDevice> getAudioDisplay(FFL::sp<AudioStream> stream) ;
	protected:
		DeviceFactory* mDeviceFactory;
		FFLPlayer* mPlayer;
		FFL::sp<AudioDevice> mAudioDevice;
		FFL::sp<VideoDevice> mVideoDevice;
	};
}
#endif
