/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  VideoTexture.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/13 
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  待渲染图片的基本信息
*/
#ifndef _VIDEO_TEXTURE_HPP_
#define _VIDEO_TEXTURE_HPP_

#include <FFL.h>
#include "VideoFormat.hpp"

namespace player {
	class VideoTexture {
	public:
		VideoTexture();
		~VideoTexture();

	public:
		VideoFormat* getVideoFormat();
	public:
		uint32_t mStreamId;
		//
		//  计算的显示时间
		//
		int64_t mRenderus;

		//
		//  显示的时间，持续的时长
		//
		int64_t mPts;
		int64_t mDuration;

		//
		//图片的宽度，高度，格式
		//
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mPixFormat;
		VideoFormat mFormat;
		//
		//  宽高比  ,可能跟视频宽度，高度不成比例
		//
		//av_guess_sample_aspect_ratio
		uint32_t mSampleAspectRatioNum;
		uint32_t mSampleAspectRatioDen;


		//
		//  图片数据，有几片，对齐大小，起始指针
		//		
		uint16_t mPlanesCount;
		uint16_t mPitches[4];
		uint8_t *mPixels[4];
	};
}

#endif