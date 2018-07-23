/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  AudioSample.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/07
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  一帧采样结构
*
*/
#ifndef _AUDIO_SAMPLE_HPP_
#define _AUDIO_SAMPLE_HPP_
#include <FFL.h>
#include "FFMpeg.hpp"
#include "AudioFormat.hpp"

namespace player {
	//
	//  音频的采样
	//
	class AudioSample {
	public:
		AudioSample();
		~AudioSample();
		//
		//  申请内存，
		//  channelNum：通道数
		//  sampleNum:采样数
		//
		void allocData(uint32_t channelNum, uint32_t sampleNum);
		//
		//  删除申请的内存
		//
		void freeData();
		//
		//  填充数据，跟allocData不一样的模式，这个填充的数据不需要释放
		//
		void fillData(uint8_t **data,int32_t linesize, uint32_t sampleNum);
		//
		//  把r中的数据移动过来,r中的数据就会清空
		//
		void moveData(AudioSample& r);
		//
		//  采样的格式
		//
		void setAudioFormat(const AudioFormat& fmt);
		void getAudioFormat(AudioFormat& fmt);
	public:
		//
		//视频流id
		//
		uint32_t mStreamId;
		//
		// 采样格式（深度），通道数，通道布局，采样个数,采样率
		//
		AVSampleFormat mFormat;
		uint32_t mChannel;
		uint32_t mFreq;
		//
		//  显示的时间，持续的时长
		//
		int64_t  mPts;
		int64_t  mDuration;			
		//
		// 采样点数量
		// 片大小，pcm数据
		//			
		uint32_t mSampleNum;
		int mLinesize;
		uint8_t **mData;		
	private:
		//
		//  是否关闭的时候自动释放内存
		//
		bool mAuotoDelete;		
	};
}


#endif