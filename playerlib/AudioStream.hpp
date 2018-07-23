/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  Audioreader/Stream.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/22 
*  https://github.com/zhenfei2016/FFL-v2.git
*
*
*/
#ifndef _AUDIO_STREAM_HPP_
#define _AUDIO_STREAM_HPP_

#include "reader/Stream.hpp"
#include "AudioFormat.hpp"

namespace player {
	class AudioStream : public reader::Stream {
	public:
		AudioStream();		
		virtual ~AudioStream();	
		//
		// 获取这个流的格式
		//
		virtual void getFormat(AudioFormat& fmt)=0;
	};
}

#endif
