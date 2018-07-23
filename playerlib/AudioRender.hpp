/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  AudioRender.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/07
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  声音的渲染类
*
*/

#ifndef _AUDIO_RENDER_HPP_
#define _AUDIO_RENDER_HPP_

#include "Render.hpp"
#include "VideoSurface.hpp"
#include "VideoTexture.hpp"
#include "AudioSample.hpp"
#include "Statistic.hpp"
#include "TimestampExtrapolator.hpp"

namespace player {
	class AudioResample;
	class AudioDevice;
	class AudioRender : public Render {
	public:
		AudioRender(FFL::sp<AudioDevice> device);
		~AudioRender();	

		void pause();
		void resume();
		void onCreate();
	public:
		//
		//  获取渲染时钟，可以改变时钟速度
		//
		virtual FFL::sp<FFL::Clock> getRenderClock();
		//
		//   外部setDataInput时候调用此函数，创建对应conn
		//
		virtual FFL::sp<FFL::PipelineConnector > onCreateConnector(const OutputInterface& output,
			const InputInterface& input,void* userdata);


		bool handleReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);
	private:
		//
		//  开始显示
		//
		virtual void onShowSamples(AudioSample* samples);

	protected:
		IStatisticAudioRender* mStatistic;	
		TimestampExtrapolator* mTimestampExtrapolator;

		FFL::sp<AudioDevice> mDevice;

		int64_t mFrameIndex;		
		FFL::sp<FFL::Clock> mClock;

		//
		// 是否需要重置同步时钟
		//
		volatile bool mResetSyncClock;
	};
}

#endif