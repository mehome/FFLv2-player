/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Window.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/10
*
*  渲染窗口
*
*/

#include "VideoSurface.hpp"

namespace player {
	VideoSurface::VideoSurface():mHandle(NULL){
	}

	VideoSurface::~VideoSurface() {
	}

	void VideoSurface::setHandle(SurfaceHandle handle) {
		mHandle = handle;
	}

	SurfaceHandle VideoSurface::getHandle() {
		return mHandle;
	}
	//
	//  获取surface的当前格式
	//
	void  VideoSurface::getFormat(VideoFormat& fmt){
        fmt=mVideoFormat;
	}
	//
	//  设置窗口的宽度，高度
	//
	void VideoSurface::setWindowSize(int32_t widht, int32_t height) {
        mVideoFormat.mWidht=widht;
        mVideoFormat.mHeight=height;
	}
	bool VideoSurface::getWindowSize(int32_t& widht, int32_t& height){
        widht=mVideoFormat.mWidht;
        height=mVideoFormat.mHeight;
		return  false;

	}
}
