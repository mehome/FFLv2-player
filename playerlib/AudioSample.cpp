/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Sample.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/07
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  音频采样结构
*
*/
#include "AudioSample.hpp"

namespace player {

	AudioSample::AudioSample() :mAuotoDelete(false) {
		//
		//  显示的时间，持续的时长
		//
		mPts = -1;
		mDuration = -1;
		//
		//
		// 采样格式，通道数，采样个数,采样率
		//
		mFormat = AV_SAMPLE_FMT_NONE;
		mChannel = 0;		
		mSampleNum = 0;
		mFreq = 0;

		mLinesize=0;
		mData = NULL;
	}

	AudioSample::~AudioSample() {
		freeData();
	}

	//
	//  申请内存，
	//  channelNum：通道数
	//  sampleNum:采样数
	//
	void AudioSample::allocData(uint32_t channelNum, uint32_t sampleNum) {
		av_samples_alloc_array_and_samples(
			(uint8_t***)&(mData), &(mLinesize),
			channelNum,
			sampleNum,
			(AVSampleFormat)mFormat, 0);
		mChannel = channelNum;
		mAuotoDelete = true;
	}
	//
	//  删除申请的内存
	//
	void AudioSample::freeData() {
		if (mAuotoDelete) {
			av_freep(mData);
			mAuotoDelete = false;
		}
	}
	//
	//  填充数据，跟allocData不一样的模式，这个填充的数据不需要释放
	//
	void AudioSample::fillData(uint8_t **data, int32_t linesize, uint32_t sampleNum) {
		freeData();

		mData = data;
		mLinesize = linesize;
		mSampleNum=sampleNum;
		mAuotoDelete = false;
	}
	//
	//  把r中的数据移动过来
	//
	void AudioSample::moveData(AudioSample& r) {
		freeData();

		mData = r.mData;
		r.mData = 0;

		mLinesize = r.mLinesize;
		r.mLinesize = 0;

		mSampleNum = r.mSampleNum;
		r.mSampleNum = 0;

		mAuotoDelete = r.mAuotoDelete;
		r.mAuotoDelete = false;
	}
	//
	//  采样的格式
	//
	void AudioSample::setAudioFormat(const AudioFormat& fmt) {
		mChannel=fmt.mChannelNum ;
		mFormat=fmt.mFormat;
		mFreq=fmt.mFreq;
	}
	void AudioSample::getAudioFormat(AudioFormat& fmt) {
		fmt.mChannelNum = mChannel;
		fmt.mFormat = mFormat;
		fmt.mFreq = mFreq;
	}
}
