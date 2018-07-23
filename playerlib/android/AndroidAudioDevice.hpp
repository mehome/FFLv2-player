/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  AndroidAudioDevice.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/28 
*  https://github.com/zhenfei2016/FFL-v2.git
*  android下播放音频的封装
*
*/#ifndef _ANDROID_AUDIO_DEVICE_HPP_
#define _ANDROID_AUDIO_DEVICE_HPP_
#include "AudioDevice.hpp"
#include "AudioDataCache.hpp"
#include <FFL.h>
#include <utils/FFL_ByteBuffer.hpp>
#include <pipeline/FFL_PipelineEvent.hpp>

namespace android {
	class AudioTrackThread;
	class AndroidAudioDevice : public player::AudioDevice {
	public:
		AndroidAudioDevice();
		virtual ~AndroidAudioDevice();
	public:
		//
		//  获取支持的格式
		//  wanted: 如果为nUll则返回所有支持的格式
		//           非null 返回跟他匹配的 
		//  fmtList: 返回支持的格式list
		//
		virtual void getSupportFormat(const player::AudioFormat* wanted, FFL::List<player::AudioFormat>& fmtList) ;
		virtual bool isSupportFormat(const player::AudioFormat* wanted) ;
		//
		// 设置缓冲多长时间的数据
		//
		virtual void setCacheUs(int64_t us) ;
		//
		//  打开关闭音频设备
		//
		virtual bool onOpen(const player::AudioFormat& wanted, int32_t sampleNum, player::AudioFormat& obtained) ;
		virtual void onClose() ;
		//
		//获取当前打开的格式，如果未打开则返回null
		//
		virtual player::AudioFormat* getOpenFormat() ;
		//
		//  写一帧数据
		//
		virtual bool writeFrame(player::AudioSample* samples) ;
		//
		//  获取播放设备硬件的延迟
		//
		virtual int64_t getDeviceDelayUs() ;
		//
		// 获取缓冲的数据量，还没有播放
		//
		virtual int64_t getCacheBytes() ;
		virtual int64_t getCacheDelay(FFL::TimeBase& base) ;
		//
		//  清空缓冲的数据
		//
		virtual int64_t clearCache() ;
		//
		// 获取播放中的音频的pts
		//
		virtual int64_t getRenderingPts() ;

    private:
        void handleOpenAudioTrack(const FFL::sp<FFL::PipelineEvent>& event);
        void handleCloseAudioTrack(const FFL::sp<FFL::PipelineEvent>& event);
        void handleWriteAudioTrack(const FFL::sp<FFL::PipelineEvent>& event);
	protected:
        player::AudioFormat mCurrentFormat;
        friend class AudioTrackThread;
        FFL::sp<AudioTrackThread> mAudioTrackThread;
        //
        //  cache多长时间的数据
        //
        int64_t mCacheUs;
        //
        //  cache音频数据
        //
        player::AudioDataCache mDataCache;
    private:
        FFL::sp<FFL::PipelineEvent> mEventOpen;
        FFL::sp<FFL::PipelineEvent> mEventClose;
        FFL::sp<FFL::PipelineEvent> mEventWrite;

    };
}
#endif
