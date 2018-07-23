/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  NodeFFMpegVideoDecoder.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/24
*
*  视频解码
*
*/

#pragma once
#include "FFMpegDecoder.hpp"

namespace player {
	class VideoStream;
	class NodeFFMpegVideoDecoder : public NodeFFMpegDecoder {
	public:
		NodeFFMpegVideoDecoder(VideoStream* stream, AVCodecContext* ctx);
		~NodeFFMpegVideoDecoder();
	protected:
		//
		//   外部setDataInput时候调用此函数，创建对应conn
		//
		virtual FFL::sp<FFL::PipelineConnector > onCreateConnector(
			const OutputInterface& output,
			const InputInterface& input,
			void* userdata);

		void correctTimestamp(message::FFMpegVideoFrame* texture);
	private:
		//
		//  解码出来的图片 ,frame通过av_frame_alloc申请出来的
		//  使用完这个frame后，会自动的 
		//  av_frame_unref(frame);
		//  av_frame_free(&frame);
		//
		void handleDecodedFrame(AVFrame* frame, int64_t trackId);
		//
		//  接收到eof消息
		//
		void handleEOF(const FFL::sp<FFL::PipelineMessage>& eof);
	public:
		VideoStream* mVideoStream;
		//
		// 缓存的数据包
		//
		FFL::sp<FFL::PipelineMessageCache> mMessageCache;
	};

}