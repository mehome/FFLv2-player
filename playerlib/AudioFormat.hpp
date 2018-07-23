/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  AudioFormat.hpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/06/13 
 *  https://github.com/zhenfei2016/FFL-v2.git
 *
 *  音频采集的格式信息
 *
*/
#ifndef _AUDIOSAMPLEFORMAT_HPP_
#define _AUDIOSAMPLEFORMAT_HPP_

#include "FFMpeg.hpp"
#define DEBUG_AUDIO 0
#if DEBUG_AUDIO
#include <utils/FFL_String.hpp>
#endif
namespace player {
	//
	//  音频采样格式信息
	//
	class AudioFormat {
	public:
		AudioFormat();
		//
		// 重置一下所有的数值
		//
		void reset();
		//
		//  比较是否相等
		//
		bool equal(const AudioFormat& r) const;	
		bool operator==(const AudioFormat& r) const;
		//
		// 赋值操作
		//
		AudioFormat& operator=(const AudioFormat& r);
	public:
		//
		//  获取通道数
		//
		uint32_t getChannelNum() const {
			return mChannelNum;
		}
		//
		//  获取声道布局
		//
		int64_t getChannelLayout();
		//
		// 获取一个采样点的大小
		//
		uint32_t getPerFrameSize();
		//
		//  输出当前的所有信息
		//
		const char* dump();
	public:
		//
		// 采样率，采样深度，声道数，速度
		//
		uint32_t mFreq;			
		AVSampleFormat mFormat;
		uint32_t mChannelNum;

#if DEBUG_AUDIO
		FFL::String mDumpInfo;
#endif
	};

	//
	//  获取播放速度改变后需要补偿的采样数，可能变大变小
	//  sampleNum:原始正常情况下的采样数
	//  speed :播放速度， 正常速度为100 , <100变慢 , >100变快
	//
	//
	//  正常的播放速度
	//
    #define SPEED_NORAML 100
	int32_t getCompensationSampleNum(uint32_t sampleNum, uint32_t speed);
}

#endif
