/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  AudioDevice.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/22 
*  https://github.com/zhenfei2016/FFL-v2.git
*  音频设备的基类
*
*/
#ifndef _AUDIO_DEVICE_HPP_
#define _AUDIO_DEVICE_HPP_

#include <ref/FFL_Ref.hpp>
#include <utils/FFL_List.hpp>
#include <utils/FFL_Clock.hpp>
#include <thread/FFL_Mutex.hpp>
#include "AudioFormat.hpp"
#include "AudioSample.hpp"

namespace player {
	class AudioRender;
	class AudioDevice : public FFL::RefBase{
	public:		
		AudioDevice();
		virtual ~AudioDevice();
		//
		//  获取支持的格式
		//  wanted: 如果为nUll则返回所有支持的格式
		//           非null 返回跟他匹配的 
		//  fmtList: 返回支持的格式list
		//
		virtual void getSupportFormat(const player::AudioFormat* wanted,FFL::List<player::AudioFormat>& fmtList)=0;
		virtual bool isSupportFormat(const player::AudioFormat* wanted) = 0;
		//
		// 设置缓冲多长时间的数据
		//
		virtual void setCacheUs(int64_t us) = 0;
		//
		//  打开关闭音频设备
		//
		bool open(const player::AudioFormat& wanted,int32_t sampleNum, player::AudioFormat& obtained);
		void close();
		bool isOpend();

		virtual bool onOpen(const player::AudioFormat& wanted, int32_t sampleNum, player::AudioFormat& obtained) = 0;
		virtual void onClose() = 0;

		//
		//获取当前打开的格式，如果未打开则返回null
		//
		virtual AudioFormat* getOpenFormat()=0;
		//
		//  写一帧数据
		//
		virtual bool writeFrame(player::AudioSample* samples)=0;
		//
		//  获取播放设备硬件的延迟
		//
		virtual int64_t getDeviceDelayUs()=0;
		//
		// 获取缓冲的数据量，还没有播放
		//
		virtual int64_t getCacheBytes()=0;	
		virtual int64_t getCacheDelay(FFL::TimeBase& base) = 0;
		//
		//  清空缓冲的数据
		//
		virtual int64_t clearCache() = 0;
		//
		// 获取render
		//
		virtual FFL::sp<player::AudioRender> getRender(void* userdata) ;
		//
		// 获取播放中的音频的pts
		//
		virtual int64_t getRenderingPts()=0;
		//
		//  设置，获取音量
		//
		virtual void setVolume(int32_t volume);
		virtual int32_t getVolume();
	protected:
		FFL::sp<player::AudioRender> mAudioRender;

		//
		// 音量
		//
		uint8_t mVolume;		

		FFL::CMutex mStateLock;
		//
		// 是否已经打开音频设备了
		//
		uint8_t mIsOpend;
	};
}
#endif