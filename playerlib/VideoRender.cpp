/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  VideoRender.cpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/05/25 
 *  
 *  视频渲染
 *
*/
#include "VideoRender.hpp"
#include "MessageFFMpegFrame.hpp"
#include "VideoDevice.hpp"
#include "PlayerCore.hpp"
#include "PlayerConstant.hpp"
#include <pipeline/FFL_PipelineAsyncConnectorFixedsize.hpp>

namespace player {
	VideoRender::VideoRender(FFL::sp<VideoDevice> device):mStatistic(NULL){
		setName("videoRender");
		mDevice = device;
		mFrameIndex = -1;
	}
	VideoRender::~VideoRender() {		
	}
	void VideoRender::pause(){
	}
	void VideoRender::resume(){
	}
	//
	//  成功创建了node
	//
	void VideoRender::onCreate() {
		Render::onCreate();
		mStatistic = &(getOwner()->mStats);
	}

	//
	//  获取渲染时钟，可以改变时钟速度
	//
	FFL::sp<FFL::Clock> VideoRender::getRenderClock() {
		return mClock;
	}
	//
	//   外部setDataInput时候调用此函数，创建对应conn
	//
	FFL::sp<FFL::PipelineConnector > VideoRender::onCreateConnector(
		const OutputInterface& output,
		const InputInterface& input, void* userdata) {
		FFL::PipelineAsyncConnectorFixSize* conn = new FFL::PipelineAsyncConnectorFixSize(3);
		conn->setName("videoRender");
		conn->setDebug(true);
		mClock = conn->getClock();
		setSpeed(getSpeed());
		return conn;
	}
	//
	// 处理接收到的消息，如果返回false表示这个消息没有处理，
	// 返回true表示这个消息处理了，消息处理结束后必须msg->consume();
	//
	bool VideoRender::handleReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata)	{
		bool ret = false;
		switch (msg->getType())
		{
		case MSG_FFMPEG_VIDEO_FRAME:
		{
			message::FFMpegVideoFrame* frame = (message::FFMpegVideoFrame*)msg->getPayload();
			onShowTexture(&(frame->mTexture));
			msg->consume(this);
			ret = true;
		}
			break;
		case MSG_CONTROL_READER_SEEK:
		{
			int64_t flag = msg->getParam2();
			if ((flag & 0x01) == 1) {
				mDevice->clearCache();
			}			
			msg->consume(this);
			break;
		}
		case MSG_CONTROL_READER_EOF:
			event::postPlayerEvent(this, event::EVENT_VIDEO_RENDER_LAST_FRAME);
			break;
		case MSG_CONTROL_AV_SYNC:
			//
			//  进行丢帧
			//
			msg->consume(this);
			break;
		}
		
		return ret;
	}
	
	//
	//  收到待显示的纹理包
	//	
	void VideoRender::onShowTexture(VideoTexture* texture)
	{
		mStatistic->renderVideoFrame(texture->mPts, FFL_getNowUs());
		int64_t t1 = FFL_getNowUs();
		mDevice->showTexture(texture);
		int64_t t2 = FFL_getNowUs();

		FFL_LOG_DEBUG_TAG(TAG_TIMESTAMP,"VideoRender pts=%" lld64 " now=%" lld64 " render=%" lld64 " delay=%" lld64,
			texture->mPts,
			t2, texture->mRenderus, t2 - texture->mRenderus);
		mStatistic->renderVideoDelayUs(t2 - t1);

		StreamPtr stream = getOwner()->getStream(texture->mStreamId);
		if (!stream.isEmpty()) {
			FFL::TimeBase tb;
			stream->getTimebase(tb);
			stream->getSyncClock()->updateClock(texture->mPts, tb);
		}

		if (mFrameIndex == -1) {
			mFrameIndex = 0;
			event::postPlayerEvent(this, event::EVENT_VIDEO_RENDER_FIRST_FRAME);
		}
		mFrameIndex++;
	}
}
