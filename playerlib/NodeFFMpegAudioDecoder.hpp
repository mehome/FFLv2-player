/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  NodeFFMpegAudioDecoder.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/15
*
*  音频解码器
*
*/

#pragma once
#include "NodeFFMpegDecoder.hpp"

namespace player {
	class AudioStream;
	class NodeFFMpegAudioDecoder : public NodeFFMpegDecoder {
	public:
		NodeFFMpegAudioDecoder(AudioStream* stream, AVCodecContext* ctx);
		~NodeFFMpegAudioDecoder();
	protected:
		//
		//   外部setDataInput时候调用此函数，创建对应conn
		//
		virtual FFL::sp<FFL::PipelineConnector > onCreateConnector(
			const OutputInterface& output,
			const InputInterface& input,
			void* userdata);
	protected:
		void correctTimestamp(message::FFMpegAudioFrame* audioFrame, AVFrame* frame);
	private:
		//
		//  解码出来的图片 ,frame通过av_frame_alloc申请出来的
		//  使用完这个frame后，会自动的 
		//  av_frame_unref(frame);
		//  av_frame_free(&frame);
		//
		void handleDecodedFrame(AVFrame* frame);
		//
		//  接收到eof消息
		//
		void handleEOF(const FFL::sp<FFL::PipelineMessage>& eof);
	public:	
		//
		// 音频流
		//
		AudioStream* mAudioStream;
		//
		//  音频格式
		//
		AudioFormat mFormat;
		//
		// 缓存的数据包
		//
		FFL::sp<FFL::PipelineMessageCache> mMessageCache;	
	};

}