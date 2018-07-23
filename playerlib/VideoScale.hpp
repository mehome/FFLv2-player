/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  VideoScale.hpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/06/12 
 *  
 *  视频缩放，转码等
 *
*/

#ifndef _VIDEO_SCALE_HPP_
#define _VIDEO_SCALE_HPP_
#include "NodeBase.hpp"
#include "FFMpeg.hpp"
#include "VideoFormat.hpp"
#include "MessageFFMpegFrame.hpp"

namespace player {

	class SDL2Module;
	class VideoScale : public NodeBase {
	public:
		VideoScale(VideoFormat* src,VideoFormat* dst);
		~VideoScale();
	public:
		//
		//  获取缩放后的出口
		//
		OutputInterface getOutput();
	protected:
		//
		//  创建图片缩放上下文
		//  src: 源格式
		//  dst: 目标格式
		//
		bool createSws(VideoFormat* src, VideoFormat* dst);
		//
		//  删除缩放上下文
		//
		void destroySws();
		//
		//  开始缩放图片
		//
		bool scaleVideo(
			uint8_t** srcPix, int* srcLinesize, int height,
			uint8_t** dstPix, int* dstLinesize);


		void scaleVideo(message::FFMpegVideoFrame* frame);
	public:
		//
		//  成功创建了node
		//
		virtual void onCreate();

		//
		//   外部setDataInput时候调用此函数，创建对应conn
		//
		FFL::sp<FFL::PipelineConnector > onCreateConnector(
			const OutputInterface& output,
			const InputInterface& input, void* userdata);
	protected:
		//
		// 处理接收到的消息，如果返回false表示这个消息没有处理，
		// 返回true表示这个消息处理了，消息处理结束后必须msg->consume();
		//
		virtual bool handleReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);	
	private:

		OutputInterface mOutputInterface;
		//
		//  当前的sws个格式
		//
		VideoFormat mSourceFormat;
		VideoFormat mDestFormat;
		SwsContext* mSwsCtx;

		//
		// 缓存的数据包
		//
		FFL::sp<FFL::PipelineMessageCache> mMessageCache;
	};
}

#endif
