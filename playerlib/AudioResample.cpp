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

#include "AudioResample.hpp"
#include "FFMpeg.hpp"
#include "PlayerConstant.hpp"

namespace player {
	AudioResample::AudioResample():mSwrCtx(NULL){
		mSpeed = SPEED_NORAML;
		mSampleNum=0;
		mCompensationNum=0;
    }

	AudioResample::~AudioResample() {
		destroySwr();
	}
	//
	//  创建重采样上下文
	//
	bool AudioResample::createSwr(const AudioFormat* src, const AudioFormat* dst, uint32_t speed){
		if (!src || !dst) {
			return false;
		}

		if (mSwrCtx){
			if (mSrc.equal(*src) && mDst.equal(*dst) && speed==mSpeed) {
				return mSwrCtx!=NULL;				
			}
			destroySwr();		
		}
		
		mSwrCtx = swr_alloc();
		mSrc = *src;
		mDst = *dst;
		mSampleNum = 0;
		mCompensationNum = 0;
		mSpeed = speed;

		//
		//  输入数据的声道布局，采样率，采样格式
		//
		av_opt_set_int(mSwrCtx, "in_channel_layout", mSrc.getChannelLayout(), 0);
		av_opt_set_int(mSwrCtx, "in_sample_rate", mSrc.mFreq, 0);
		av_opt_set_sample_fmt(mSwrCtx, "in_sample_fmt", (AVSampleFormat)mSrc.mFormat, 0);

		//
		//  输出数据的声道布局，采样率，采样格式
		//	
		av_opt_set_int(mSwrCtx, "out_channel_layout", mDst.getChannelLayout(), 0);
		av_opt_set_int(mSwrCtx, "out_sample_rate", mDst.mFreq, 0);
		av_opt_set_sample_fmt(mSwrCtx, "out_sample_fmt", (AVSampleFormat)mDst.mFormat, 0);		
		if (swr_init(mSwrCtx) >= 0) {
			return true;
		}

		 
		FFL_LOG_WARNING("Failed to AudioResample::createSwr. ");
		swr_free(&mSwrCtx);
		mSwrCtx = 0;
		mSrc.reset();
		mDst.reset();
		return false;
	}
	//
	//  删除重采样上下文
	//
	void AudioResample::destroySwr() {
		if (mSwrCtx) {
			swr_free(&mSwrCtx);
		}
	}

	bool AudioResample::resample(const AudioSample* src, AudioSample* dst,uint32_t speed) {
		AudioFormat srcFormat;
		srcFormat.mFreq = src->mFreq;
		srcFormat.mFormat = src->mFormat;
		srcFormat.mChannelNum = src->mChannel;

		AudioFormat dstFormat;
		dstFormat.mFreq = dst->mFreq;
		dstFormat.mFormat = dst->mFormat;
		dstFormat.mChannelNum = dst->mChannel;
		//destroySwr();
		if (!createSwr(&srcFormat, &dstFormat, speed)) {
			return false;
		}
		
		//uint32_t wantSampleNum=
		//
		//  计算重采样后的采样数,
		//  根据源，目标采样率 和上次重采样缓存没处理了的
		//
		uint32_t dstSampleNum =(uint32_t) av_rescale_rnd(src->mSampleNum ,dst->mFreq,src->mFreq, AV_ROUND_UP) ;

		//
		//  如果速度改变了，则需要插值多少个采样点
		//
		if (mSpeed == SPEED_NORAML) {
			mCompensationNum = 0;
			mSampleNum = 0;
		}else{
			if(mCompensationNum==0 || mSampleNum != dstSampleNum){
				mCompensationNum=getCompensationSampleNum(dstSampleNum, mSpeed);
				mSampleNum = dstSampleNum;
			}			
		}

		if (mCompensationNum!=0 ) {			
			if (swr_set_compensation(mSwrCtx, mCompensationNum, dstSampleNum+ mCompensationNum) < 0) {
				FFL_LOG_WARNING("swr_set_compensation() failed");
			}
		}

     	//
		//  申请内存存储重采样数据
		//		
		dstSampleNum += mCompensationNum;
		dst->allocData(dstFormat.getChannelNum(), dstSampleNum);		
		int ret = swr_convert(mSwrCtx, dst->mData, dstSampleNum,
			(const uint8_t**)src->mData, src->mSampleNum);
		if (ret < 0) {
			FFL_LOG_WARNING("resample Error while converting\n");			
			return false;
		}
		dst->mSampleNum = ret;
		FFL_LOG_INFO_TAG(TAG_AUDIO,"resample src=%d dst=%d ret=%d ,delay=%d",
			src->mSampleNum,
			dstSampleNum,
			ret,
			swr_get_delay(mSwrCtx, src->mFreq));

		dst->mLinesize = (ret)* dstFormat.getPerFrameSize();
        //int size=av_samples_get_buffer_size(&(dst->mLinesize), dst->mChannel,
		//	ret, (AVSampleFormat)dst->mFormat, 0);
		return true;	
	}
}
