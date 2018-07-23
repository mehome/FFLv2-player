/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  DefaultDeviceManager.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/07/01
*  https://github.com/zhenfei2016/FFL-v2.git
*  音视频设备管理，设备的打开，关闭，获取已经缓存的音视频设备
*  默认使用的，android平台下使用 AndroidModule.hpp  ，其他平台使用SDL2Module.hpp
*
*/

#include "DefaultDeviceManager.hpp"
#if defined(ANDROID)
#include "android/AndroidModule.hpp"
#else
#include "SDL2Module.hpp"
#endif

namespace player {
	DefaultDeviceManager::DefaultDeviceManager(){
#if defined(ANDROID)
		mDeviceFactory = new android::AndroidModule();
#else
		mDeviceFactory = new SDL2Module();
#endif
	}
	DefaultDeviceManager::~DefaultDeviceManager() {
		if (!mVideoDevice.isEmpty()) {
			mVideoDevice->close();
			mVideoDevice = NULL;
		}

		if (!mAudioDevice.isEmpty()) {
			mAudioDevice->close();
			mAudioDevice = NULL;
		}

		FFL_SafeFree(mDeviceFactory);
	}
	//
	//  通过wnd创建对应的
	//
	FFL::sp<VideoDevice> DefaultDeviceManager::openVideoDisplay(SurfaceHandle wnd) {
		FFL::sp<VideoDevice> dev = mDeviceFactory->createVideoDevice(this);
		uint32_t width = 400;
		uint32_t height = 300;	
		dev->open(wnd, width, height);
		if (!mVideoDevice.isEmpty()) {
			FFL_LOG_WARNING("DefaultDeviceManager::openVideoDisplay mVideoDevice isnot null.");
		}
		mVideoDevice = dev;
		return dev;
	}
	//
	//  如果式通过已经创建的SufraceHandle 创建的VideoDevice则不会关闭这个窗口的
	//
	void DefaultDeviceManager::closeVideoDisplay() {
		if (!mVideoDevice.isEmpty()) {
			mVideoDevice->close();
			mVideoDevice = NULL;
		}
	}
	//
	//  通过fmt打开对应的音频设备
	//
	FFL::sp<AudioDevice> DefaultDeviceManager::openAudioDisplay(AudioFormat fmt) {		
		FFL::sp<AudioDevice> dev = mDeviceFactory->createAudioDevice(this);
		if(dev.isEmpty()){
			FFL_LOG_ERROR("Faild to DefaultDeviceManager.openAudioDisplay.");
			return NULL;
		}
		//
		// 启动音频设备
		//
		AudioFormat obtainedFmt;
		if (!dev->open(fmt, 1024, obtainedFmt)) {
			return NULL;
		}

		mAudioDevice = dev;
		return dev;
	}
	//
	//  关闭音频设备
	//
	void DefaultDeviceManager::closeAudioDisplay() {
		if (!mAudioDevice.isEmpty()) {
			mAudioDevice->close();
			mAudioDevice = NULL;
		}
	}
	//
	//  获取已经创建的视频设备
	//
	FFL::sp<VideoDevice> DefaultDeviceManager::getVideoDisplay(FFL::sp<VideoStream> stream) {
		return mVideoDevice;
	}
	//
	// 获取已经创建的音频设备
	//
	FFL::sp<AudioDevice> DefaultDeviceManager::getAudioDisplay(FFL::sp<AudioStream> stream) {
		return mAudioDevice;
	}
}