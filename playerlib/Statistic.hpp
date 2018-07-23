/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  Statistic.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/05/01
*
*  统计接口
*
*/

#pragma once
#include <FFL.h>
namespace player {
	class IStatisticAudioRender {
	public:
		//
		// 获取设置延迟
		//
		virtual void renderAudioDelayUs(int64_t delay) = 0;
		virtual int64_t getAudioRenderDelayUs() = 0;

		//
		//  开始渲染timestamp对应的一帧数据
		//  now:当前的本地时间
		//
		virtual void renderAudioFrame(int64_t timestamp, int64_t now) = 0;
	};

	class IStatisticVideoRender {
	public:
		//
		// 获取设置延迟
		//
		virtual void renderVideoDelayUs(int64_t delay) = 0;
		virtual int64_t getVideoRenderDelayUs() = 0;

		//
		//  开始渲染timestamp对应的一帧数据
		//  now:当前的本地时间
		//
		virtual void renderVideoFrame(int64_t timestamp, int64_t now) = 0;
	};

	class IStatisticAudioDecoder {
	public:
		//
		// 获取设置延迟
		//
		virtual void setAudioDecoderDelayUs(int64_t delay) = 0;
		virtual int64_t getAudioDecoderDelayUs() = 0;
	};

	class IStatisticVideoDecoder {
	public:
		//
		// 获取设置延迟
		//
		virtual void setVideoDecoderDelayUs(int64_t delay) = 0;
		virtual int64_t getVideoDecoderDelayUs() = 0;
	};



	class IStatisticAudioComposer {
	public:
		//
		// 获取设置延迟
		//
		virtual void setAudioComposerDelayUs(int64_t delay) = 0;
		virtual int64_t getAudioComposerDelayUs() = 0;
	};

	class IStatisticVideoComposer {
	public:
		//
		// 获取设置延迟
		//
		virtual void setVideoComposerDelayUs(int64_t delay) = 0;
		virtual int64_t getVideoComposerDelayUs() = 0;
	};


	//
	//  播放器的统计信息
	//
	class PlayerStatistic :
		public IStatisticVideoRender,
		public IStatisticAudioRender,
		public IStatisticVideoComposer,
		public IStatisticAudioComposer,
		public IStatisticVideoDecoder,
		public IStatisticAudioDecoder {
	public:
		PlayerStatistic();
		~PlayerStatistic();

		void renderAudioDelayUs(int64_t delay);
		int64_t getAudioRenderDelayUs();

		void renderVideoDelayUs(int64_t delay);
		int64_t getVideoRenderDelayUs();

		virtual void setAudioComposerDelayUs(int64_t delay);
		virtual int64_t getAudioComposerDelayUs();

		virtual void setVideoComposerDelayUs(int64_t delay);
		virtual int64_t getVideoComposerDelayUs();

		virtual void setAudioDecoderDelayUs(int64_t delay);
		virtual int64_t getAudioDecoderDelayUs();

		virtual void setVideoDecoderDelayUs(int64_t delay);
		virtual int64_t getVideoDecoderDelayUs();

		//
		//  开始渲染timestamp对应的一帧数据
		//  now:当前的本地时间
		//
		virtual void renderAudioFrame(int64_t timestamp, int64_t now);
		//
		//  开始渲染timestamp对应的一帧数据
		//  now:当前的本地时间
		//
		virtual void renderVideoFrame(int64_t timestamp, int64_t now);

	public:
		int64_t mVideoRenderDelayUs;
	};
}