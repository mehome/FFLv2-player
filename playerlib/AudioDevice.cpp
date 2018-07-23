/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  AudioDevice.cpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/22 
*  https://github.com/zhenfei2016/FFL-v2.git
*  音频设备的基类
*
*/
#include "AudioDevice.hpp"
#include "AudioRender.hpp"
namespace player {
	AudioDevice::AudioDevice():
		mVolume(255),mIsOpend(0){
	}
	AudioDevice::~AudioDevice(){
	}
	//
	//  打开关闭音频设备
	//
	bool AudioDevice::open(const player::AudioFormat& wanted, int32_t sampleNum, player::AudioFormat& obtained) {
		FFL::CMutex::Autolock l(mStateLock);
		if (mIsOpend!=0) {
			return false;
		}

		bool ok= onOpen(wanted, sampleNum, obtained);		
		mIsOpend = ok?1:0;
		return ok;
	}
	void AudioDevice::close() {
		FFL::CMutex::Autolock l(mStateLock);
		onClose();
		mAudioRender = NULL;
		mIsOpend = 0;
	}
	bool AudioDevice::isOpend() {
		FFL::CMutex::Autolock l(mStateLock);
		return mIsOpend!=0; 
	}
	//
	// 获取render
	//
	FFL::sp<AudioRender> AudioDevice::getRender(void* userdata) {
		if (mAudioRender.isEmpty()) {
			mAudioRender = new AudioRender(this);
		}
		return mAudioRender;
	}
	//
	//  设置，获取音量
	//
	void AudioDevice::setVolume(int32_t volume){
		mVolume = volume;
	}
	int32_t AudioDevice::getVolume(){
		return mVolume;
	}
}
