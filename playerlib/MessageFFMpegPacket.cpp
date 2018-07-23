/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  MessageFFMpegPacket.cpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/03/10 
 *  
 *  未解码包
 *
*/
#include "MessageFFMpegPacket.hpp"
#include "reader/Stream.hpp"

namespace message {
	FFMpegPacket::FFMpegPacket()
	{
        setType(MSG_FFMPEG_AVPACKET);
        mPacket=av_packet_alloc();
		mSerialNumber = 0;
		mIFrame = 0;
		mPacketType = 0;
    }
	FFMpegPacket::~FFMpegPacket(){
        av_packet_free(&mPacket);
    } 
	//
	//  是否i桢
	//
	bool FFMpegPacket::isIFrame() const {
		return mIFrame!=0;
	}
	//
	//  是否视频，音频桢
	//
	bool FFMpegPacket::isVideo() const {
		return mPacketType ==  STREAM_TYPE_VIDEO;
		//mPacket->flags = ;
	}
	bool FFMpegPacket::isAudio() const {
		return mPacketType == STREAM_TYPE_AUDIO;
	}
	//
	//  已经处理完成了，可以回收了		
	//
	void FFMpegPacket::consume() {
		av_packet_unref(mPacket);
	}
}
