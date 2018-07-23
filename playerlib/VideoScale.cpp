/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  VideoScale.cpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/06/12 
 *  
 *
 *
*/
#include "VideoScale.hpp"
#include "MessageTypes.hpp"
#include "MessageFFMpegFrame.hpp"
#include <pipeline/FFL_PipelineAsyncConnectorFixedsize.hpp>

namespace player {
	class FFMPegScalVideoFrame : public message::FFMpegVideoFrame{
	public:
		FFMPegScalVideoFrame(){
			mFrame=av_frame_alloc();
		}
		~FFMPegScalVideoFrame(){
			av_frame_free(&mFrame);
		}

		void allocBuffer(VideoFormat& fmt){
			if(mFrame->format != fmt.getFFMpegFormat()||
					   mFrame->width!=fmt.mWidht||
					   mFrame->height!=fmt.mHeight) {
				mFrame->format = fmt.getFFMpegFormat();
				mFrame->width = fmt.mWidht;
				mFrame->height = fmt.mHeight;
				av_frame_get_buffer(mFrame, 32);
			}
		}
		//
		//  已经处理完成了，可以回收了
		//
		void consume(){

		}

		//
		//  填充数据
		//
		void fillAvframe(AVFrame* frame){
			message::FFMpegVideoFrame::fillAvframe(mFrame);
		}
	};
	VideoScale::VideoScale(VideoFormat* src,VideoFormat* dst):mSwsCtx(NULL){
		mSourceFormat=*src;
		mDestFormat = *dst;
		mMessageCache = new FFL::PipelineMessageCache(MSG_FFMPEG_VIDEO_FRAME);
	}
	VideoScale::~VideoScale() {
		destroySws();
	}

	OutputInterface VideoScale::getOutput(){
		if(!mOutputInterface.isValid()){
			mOutputInterface=createOutputInterface();
		}
		return mOutputInterface;
	}
	//
	//  成功创建了node
	//
	void VideoScale::onCreate() {
		NodeBase::onCreate();
	}	
	//
	//  创建图片缩放上下文
	//  src: 源格式
	//  dst: 目标格式
	//
	bool VideoScale::createSws(VideoFormat* src, VideoFormat* dst)
	{
		int flags = SWS_BILINEAR;		
		int dstWidth = mDestFormat.mWidht == -1 ? mSourceFormat.mWidht : mDestFormat.mWidht;
		int dstHight = mDestFormat.mHeight == -1 ? mSourceFormat.mHeight : mDestFormat.mHeight;
		mSwsCtx = sws_getContext(mSourceFormat.mWidht,
			mSourceFormat.mHeight, (AVPixelFormat)mSourceFormat.getFFMpegFormat(),
			dstWidth,
			dstHight,(AVPixelFormat)mDestFormat.getFFMpegFormat(), flags, 0, 0, 0);
		return true;
	}
	//
	//  开始缩放图片
	//
	bool VideoScale::scaleVideo(
		uint8_t* srcPix[],int srcLinesize[],int height,
		uint8_t* dstPix[], int dstLinesize[]) {
		if (!mSwsCtx) {
			FFL_LOG_WARNING("Failed to scaleVideo");
			return false;
		}
		int ret= sws_scale(mSwsCtx,
                  (const uint8_t *const *) srcPix,
			srcLinesize,
				0,
			height,
			dstPix,
			dstLinesize);

		if(ret<0){
			FFL_LOG_WARNING("Failed to sws_scale");
		}
		return false;		
	}
	void VideoScale::scaleVideo(message::FFMpegVideoFrame* frame){
		FFMPegScalVideoFrame* texture = 0;
		FFL::sp<FFL::PipelineMessage> msg = message::createMessageFromCache(mMessageCache, &texture, MSG_FFMPEG_VIDEO_FRAME);
		texture->allocBuffer(mDestFormat);

		if(mSwsCtx==NULL) {
			createSws(&mSourceFormat, &mDestFormat);
		}
		scaleVideo((frame->mFrame->data),
				   frame->mFrame->linesize,
				   mDestFormat.mHeight,
		           texture->mFrame->data,texture->mFrame->linesize);
		texture->fillAvframe(texture->mFrame);

		texture->mTexture.mStreamId = frame->mTexture.mStreamId;
        texture->mTexture.mPts=frame->mTexture.mPts;
        texture->mTexture.mDuration=frame->mTexture.mDuration;

		if(mOutputInterface.isValid()) {
			if(FFL_OK==postMessage(mOutputInterface.mId, msg)){
				return;
			}
		}

		//
		//  发送不成功情况
		//
		msg->consume(this);
	}
	//
	//  删除缩放上下文
	//
	void VideoScale::destroySws() {
		if(mSwsCtx){
			sws_freeContext(mSwsCtx);
		}
	}
	//
	//   外部setDataInput时候调用此函数，创建对应conn
	//
	FFL::sp<FFL::PipelineConnector > VideoScale::onCreateConnector(
		const OutputInterface& output,
		const InputInterface& input, void* userdata) {
		FFL::PipelineAsyncConnectorFixSize* conn = new FFL::PipelineAsyncConnectorFixSize(5);		
		return conn;
	}
	//
	// 处理接收到的消息，如果返回false表示这个消息没有处理，
	// 返回true表示这个消息处理了，消息处理结束后必须msg->consume();
	//
	bool VideoScale::handleReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata){
		bool ret = false;
		switch (msg->getType())
		{
			case MSG_FFMPEG_VIDEO_FRAME: {
				message::FFMpegVideoFrame *frame = (message::FFMpegVideoFrame *) msg->getPayload();
				scaleVideo(frame);
				frame->consume();
				ret = true;
				break;
			}
			default:
			break;
		}
		return ret;
	}


}
