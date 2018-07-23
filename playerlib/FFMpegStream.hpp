/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFMpegreader/Stream.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/16
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  流基本信息
*
*/

#pragma once

#include "FFMpeg.hpp"
#include "AudioStream.hpp"
#include "VideoStream.hpp"

namespace player {
	template< class BASESTREAM>
	class FFMpegStream : public BASESTREAM {
	public:
		FFMpegStream(AVStream* ffmpeg) :mFFMpegStream(ffmpeg) {
		}
		~FFMpegStream() {
		}
		//
		// 获取流索引
		//
		virtual uint32_t getIndex() const {
			return mFFMpegStream->index;
		}
		//
		// 获取流类型
		//
		virtual uint32_t getStreamType() const {
			switch (avcodec_get_type(mFFMpegStream->codecpar->codec_id))
			{
			case AVMEDIA_TYPE_AUDIO:
				return STREAM_TYPE_AUDIO;
			case AVMEDIA_TYPE_VIDEO:
				return STREAM_TYPE_VIDEO;
			case AVMEDIA_TYPE_SUBTITLE:
				return STREAM_TYPE_SUBTITLE;
			default:
				return STREAM_TYPE_OTHER;
			}  
		}
		//
		// 获取流开始的时间戳，以getTimebase为单位的
		//
		virtual int64_t getStartTime() const {
			return mFFMpegStream->start_time == -1 ? 0 : mFFMpegStream->start_time;
		}
		//
		//  获取时基单位
		//
		virtual void getTimebase(FFL::TimeBase& tb) const {
			tb.mDen = mFFMpegStream->time_base.den;
			tb.mNum = mFFMpegStream->time_base.num;
		}
	protected:		
		AVStream* mFFMpegStream;
	};




	class FFMpegAudioStream : public FFMpegStream<AudioStream> {
	public:
		FFMpegAudioStream(AVStream* ffmpeg);
		~FFMpegAudioStream();
		//
		// 获取这个流的格式
		//
		virtual void getFormat(AudioFormat& fmt);
		//
		//  创建这个流需要的解码器
		//
		FFL::sp<Decoder> createDecoder(player::PlayerCore* core);
	private:
		AVCodecContext* mFFMpegCodecCtx;
		FFL::sp<Decoder> mDecoder;
	};


	class FFMpegVideoStream : public FFMpegStream<VideoStream> {
	public:
		FFMpegVideoStream(AVStream* ffmpeg);
		~FFMpegVideoStream();	
		//
		// 获取宽度，高度
		//
		virtual void getSize(uint32_t& width, uint32_t& height);
		//
		//  创建这个流需要的解码器
		//
		FFL::sp<Decoder> createDecoder(player::PlayerCore* core);
	private:
		AVCodecContext* mFFMpegCodecCtx;
		FFL::sp<Decoder> mDecoder;
	};
}