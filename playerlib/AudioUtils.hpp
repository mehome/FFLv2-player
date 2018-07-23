/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  AudioUtils.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/06/22
*  https://github.com/zhenfei2016/FFL-v2.git
*  音频处理的通用函数
*
*/
#ifndef _AUDIO_UTILS_HPP_
#define _AUDIO_UTILS_HPP_
#include <FFL.h>
namespace player {	
	//
	//  获取希望的采样数
	//  sampleNum:原始采样数
	//  speed：播放速度
	//  返回这个speed速度情况下，需要的采样点数量
	//
	int32_t getWantSampleNum(uint32_t sampleNum, uint32_t speed);
	//
	//  获取希望的采样数
	//  sampleNum:原始采样数
	//  speed：播放速度
	//  返回这个speed速度情况下，需要的采样点数量
	//
	//class AudioFormat;
	//int32_t getWantSampleNum(uint32_t sampleNum,const AudioFormat& dst, int32_t delayUs, uint32_t speed);

	//
	//  混音src，dst结果到dst中
	//
	void audioMix(uint8_t* dst,uint8_t* src, uint32_t* len);
}

#endif
