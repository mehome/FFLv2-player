/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  MessageFFMpegFrame.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/11
*
*  视频，音频解码后的结构包装
*
*/

#ifndef _FFMPEGVIDEOFRAME_HPP_
#define _FFMPEGVIDEOFRAME_HPP_

#include "MessageBase.hpp"
#include "FFMpeg.hpp"
#include "VideoTexture.hpp"
#include "AudioSample.hpp"

namespace message {
	//
	//   解码完的的数据包
	//
	class FFMpegVideoFrame : public MessageBase{
	public:		
		FFMpegVideoFrame();
		~FFMpegVideoFrame();
		//
		//  已经处理完成了，可以回收了		
		//
		virtual  void consume();
		//
		//  填充数据
		//
		virtual  void fillAvframe(AVFrame* frame);
	public:
		player::VideoTexture mTexture;
		AVFrame* mFrame;
	};


	
	class FFMpegAudioFrame : public MessageBase
	{
	public:
		FFMpegAudioFrame();
		~FFMpegAudioFrame();
     	//
		//  已经处理完成了，可以回收了		
		//
		void consume();
		//
		//  填充数据
		//
		void fillAvframe(AVFrame* frame);
	public:		
		player::AudioSample  mSamples;
		AVFrame* mFrame;
	};
}

#endif