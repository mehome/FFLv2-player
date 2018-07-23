/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  AudioResample.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/07
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  声音的重采样
*
*/
#ifndef _AUDIO_RESAMPLE_HPP_
#define _AUDIO_RESAMPLE_HPP_

#include "AudioSample.hpp"
#include "FFMpeg.hpp"
#include "AudioFormat.hpp"

namespace player {
	class AudioResample  {	
	public:
		AudioResample();
		~AudioResample();
	protected:
		//
		//  创建重采样上下文
		//
		bool createSwr(const AudioFormat* src,const AudioFormat* dst,uint32_t speed);
		//
		//  删除重采样上下文
		//
		void destroySwr();
	public:
		//
		//  speed  :当前音频的播放速度100正常速度
		//   in:    输入的采样数据
		//   out:   输出的重采样数据
		//
		bool resample(const AudioSample* in , AudioSample* out,uint32_t speed);		
	protected:
		//
		//  ffmpeg中重采样的
		//
		SwrContext *mSwrCtx;		
		//
		//  当前的源格式，目标格式
		//
		AudioFormat mSrc;
		AudioFormat mDst;
		//
		// 播放速度,
		// 速度改变情况下 mSampleNum个采样 ，对应CompensationNum个补偿
		//
		uint32_t mSpeed;

		uint32_t mSampleNum;
		int32_t  mCompensationNum;
	};
}

#endif
