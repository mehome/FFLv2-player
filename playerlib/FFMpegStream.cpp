/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFMpegStream.cpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/04/16 
 *  https://github.com/zhenfei2016/FFL-v2.git
 *
 *  流基本信息
 *
*/
#include "FFMpegStream.hpp"
#include "FFMpegVideoDecoder.hpp"
#include "FFMpegAudioDecoder.hpp"
#include "FFMpegDecoder.hpp"

namespace player {
	static AVCodecContext* openCodec(AVStream* stream)
	{
		AVCodec* codec = NULL;
		AVCodecContext* codecCtx = NULL;		
		codec = avcodec_find_decoder(stream->codecpar->codec_id);
		if (!codec)
		{
			FFL_LOG_WARNING("failed to find codec\n");
			return NULL;
		}


		codecCtx = avcodec_alloc_context3(codec);
		if (!codecCtx)
		{
			FFL_LOG_WARNING("Failed to allocate the codec context\n");
			goto fail;
		}

		if (avcodec_parameters_to_context(codecCtx, stream->codecpar) < 0) {
			FFL_LOG_WARNING("Failed to copy codec parameters to decoder context\n");
			goto fail;
		}

		if (avcodec_open2(codecCtx, codec, NULL) < 0)
		{
			FFL_LOG_WARNING("Failed to open codec\n");
			goto fail;
		}

		return codecCtx;
	fail:

		if (codecCtx)
			avcodec_free_context(&codecCtx);
		return  NULL;
	}

	FFMpegAudioStream::FFMpegAudioStream(AVStream* ffmpeg)
	      :FFMpegStream(ffmpeg),
		mFFMpegCodecCtx(NULL){
		mFFMpegCodecCtx = openCodec(mFFMpegStream);
	}
	FFMpegAudioStream::~FFMpegAudioStream() {
		if (mFFMpegCodecCtx)
		   avcodec_free_context(&mFFMpegCodecCtx);
	}
	// 获取这个流的格式
	//
	void FFMpegAudioStream::getFormat(AudioFormat& fmt) {
		fmt.mFreq= mFFMpegCodecCtx->sample_rate;
		fmt.mChannelNum=mFFMpegCodecCtx->channels;
		fmt.mFormat = mFFMpegCodecCtx->sample_fmt;
	}
	//
	//  创建这个流需要的解码器
	//
	FFL::sp<Decoder> FFMpegAudioStream::createDecoder(player::PlayerCore* core) {
		if (!mDecoder.isEmpty()) {
			return mDecoder;
		}

	    if (getStreamType() == STREAM_TYPE_AUDIO) {			
			if (mFFMpegCodecCtx == NULL) {
				FFL_LOG_WARNING("Failed to FFMpegAudioStream::createDecoder id=%d", mFFMpegStream->codecpar->codec_id);
				return NULL;
			}
			mDecoder = new NodeFFMpegAudioDecoder(this, mFFMpegCodecCtx);			
			mDecoder->create(core);

			InputInterface inputInterface;
			mDecoder->connectSource(mSource, mSource.mName.c_str(), inputInterface, 0);
		}
		return mDecoder;
	}
	
	FFMpegVideoStream::FFMpegVideoStream(AVStream* ffmpeg) :
		FFMpegStream(ffmpeg),
		mFFMpegCodecCtx(NULL) {
		mFFMpegCodecCtx = openCodec(mFFMpegStream);
	}
	FFMpegVideoStream::~FFMpegVideoStream() {
		if(mFFMpegCodecCtx)
		   avcodec_free_context(&mFFMpegCodecCtx);
	}
	//
	// 获取宽度，高度
	//
	void FFMpegVideoStream::getSize(uint32_t& width, uint32_t& height) {
		width = mFFMpegStream->codecpar->width;
		height = mFFMpegStream->codecpar->height;
	}
	//
	//  创建这个流需要的解码器
	//
	FFL::sp<Decoder> FFMpegVideoStream::createDecoder(player::PlayerCore* core) {
		if (!mDecoder.isEmpty()) {
			return mDecoder;
		}

		if (getStreamType() == STREAM_TYPE_VIDEO) {			
			if (mFFMpegCodecCtx == NULL) {
				FFL_LOG_WARNING("Failed to FFMpegVideoStream::createDecoder id=%d", mFFMpegStream->codecpar->codec_id);
				return NULL;
			}
			mDecoder = new NodeFFMpegVideoDecoder(this, mFFMpegCodecCtx);		
			mDecoder->create(core);

			InputInterface inputInterface;
			mDecoder->connectSource(mSource, mSource.mName.c_str(), inputInterface, 0);			
		}
		return mDecoder;
	}

}