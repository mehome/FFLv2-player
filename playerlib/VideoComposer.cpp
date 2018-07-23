/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  VideoComposer.cpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/05/19 
 *  
 *  视频合成
 *
*/
#include "VideoComposer.hpp"
#include "MessageFFMpegFrame.hpp"
#include "VideoTexture.hpp"
#include "PlayerCore.hpp"
#include "reader/Stream.hpp"
#include "PlayerConstant.hpp"
#include "TimestampExtrapolator.hpp"
#include "SyncUtils.hpp"
#include "VideoRender.hpp"
#include "VideoScale.hpp"
#include <pipeline/FFL_PipelineAsyncConnectorFixedsize.hpp>

namespace player {
	VideoComposer::VideoComposer():mTimestampExtrapolator(NULL){
		setName("VideoComposer");
		mTimestampExtrapolator = new TimestampExtrapolator("video");
	}

	VideoComposer::~VideoComposer(){
		FFL_SafeFree(mTimestampExtrapolator);
	}
	//
	//  成功创建了node
	//
	void VideoComposer::onCreate() {
		Composer::onCreate();
		mStatistic = &(getOwner()->mStats);
	}


	//
	//   外部setDataInput时候调用此函数，创建对应conn
	//
	FFL::sp<FFL::PipelineConnector > VideoComposer::onCreateConnector(
		const OutputInterface& output,
		const InputInterface& input,
		void* userdata) {
        FFL::PipelineConnector* conn;
        if(mConnector.isEmpty()){
            mConnector=new FFL::PipelineAsyncConnectorFixSize(5);
            conn=mConnector.get();
        }else{
			//FFL::sp<FFL::PipelineLooper> looper=mConnector->getLooper();
			conn=new FFL::PipelineAsyncConnectorFixSize(5);
        }
		return conn;
	}
	//
	//
	// 处理接收到的消息，如果返回false表示这个消息没有处理，
	// 返回true表示这个消息处理了，消息处理结束后必须msg->consume();
	//
	bool VideoComposer::handleReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata){
		if (msg.isEmpty()) {			
			return true;
		}
		
		switch (msg->getType())
		{
		case MSG_FFMPEG_VIDEO_FRAME:
		{
			
			message::FFMpegVideoFrame* frame = (message::FFMpegVideoFrame*)msg->getPayload();
			handleTexture(msg, &(frame->mTexture));
		}
			break;
		case MSG_FFMPEG_VIDEO_FRAME_SCALE:
		{
			message::FFMpegVideoFrame* frame = (message::FFMpegVideoFrame*)msg->getPayload();
			handleTexture(msg, &(frame->mTexture));
		}
		break;
		case MSG_CONTROL_READER_SEEK:
		{
			int64_t flag = msg->getParam2();
			if ((flag & 0x01) == 1) {
				clearMessage(mOutputToRenderInterface.mId);
			}

			if (FFL_OK != postMessage(mOutputToRenderInterface.mId, msg)) {
				msg->consume(this);
			}
			mTimestampExtrapolator->reset();
			break;
		}
			break;
		case MSG_CONTROL_READER_EOF:
			handleEOF(msg);
			msg->consume(this);
			break;
		default:			
			msg->consume(this);
			break;
		}

		return true;
	}	
	//
	//  计算多长时间后播放这一桢
	//
	int64_t VideoComposer::getDelay(VideoTexture* texture,OptMode& mode) {
		StreamPtr stream = getOwner()->getStream(texture->mStreamId);
		stream->getTimebase(mTb);

		uint32_t speed=getOwner()->getSpeed();
		mTimestampExtrapolator->setSpeed(speed);

		int64_t lastFrameDuration = 0;
		int64_t delay = mTimestampExtrapolator->getDelayAndUpdate(texture->mPts, mTb,&lastFrameDuration);
		if (delay < 0) {
			delay += 0;
		}

		texture->mRenderus = FFL_getNowUs() + delay * 100 / speed;
		delay -= mStatistic->getVideoRenderDelayUs();
		
		delay -= lastFrameDuration;
		
		int64_t correctDelay = correctVideoDelay(getOwner()->getMasterClock(),
			stream->getSyncClock(),
			lastFrameDuration,&mode);

		FFL_LOG_DEBUG_TAG(TAG_TIMESTAMP,
			"VideoComposer pts=%" lld64 " delay=%" lld64 " correctDelay=%" lld64,
			texture->mPts, delay, correctDelay);

		return delay+correctDelay;
	}
	//
	//  收到待显示的纹理包
	//	
	void VideoComposer::handleTexture(const FFL::sp<FFL::PipelineMessage>& msg,VideoTexture* texture) {
        //
        //  查看渲染是否支持这个格式，如果不支持则需要进行缩放
        //
        FFL::sp<VideoDevice> renderDevice=getOwner()->getDeviceManager()->getVideoDisplay(NULL);
		if (!renderDevice.isEmpty()) {
			VideoFormat* videoFormat = texture->getVideoFormat();
			if (videoFormat) {
				if (!renderDevice->isSupportFormat(videoFormat)) {
					VideoFormat dstFormat;
					if (renderDevice->getOptimalFormat(videoFormat, &dstFormat)) {
						scaleVideo(msg, texture, videoFormat, &dstFormat);
					}
					else {
						FFL_LOG_WARNING("Failed to getOptimalFormat ");
					}
					return;
				}
			}
		}

        //
        //  计算时间发送到渲染节点
        //
		OptMode mode;
		int64_t delay = getDelay(texture, mode);
		if (delay < 0 /*|| mode==DROP_FRAME*/) {
			FFL_LOG_WARNING_TAG(TAG_TIMESTAMP,"VideoComposer drop frame pts=%" lld64,texture->mPts);
			StreamPtr stream = getOwner()->getStream(texture->mStreamId);
			if (!stream.isEmpty()) {
				FFL::TimeBase tb;
				stream->getTimebase(tb);
				stream->getSyncClock()->updateClock(texture->mPts, tb);
			}

			//msg->consume(this);
			//return;
			delay=0;
		}		
		//
		// 发送到这个输出接口上
		//		
		postMessageDelayToRender(msg, delay);
	}	
	//
	//  接收到eof消息
	//
	void VideoComposer::handleEOF(const FFL::sp<FFL::PipelineMessage>& eof) {
		postMessageDelayToRender(eof,-1);
	}
    //
    // 进行缩放，从src格式转到dst格式,异步操作
    //
    void VideoComposer::scaleVideo(const FFL::sp<FFL::PipelineMessage>& msg,VideoTexture* texture,VideoFormat* src,VideoFormat* dst){
        if(!createScale(texture->mStreamId,src,dst)){
            msg->consume(this);
            return;
        }

        if(FFL_OK!=postMessage(mScaleOutput.mId,msg)){
            msg->consume(this);
        }
    }

    //
    //  设置这个合成器的输出到那个一个渲染器中
    //
    bool VideoComposer::createScale(int32_t streamId, VideoFormat* src,VideoFormat* dst) {
        if (mScaleOutput.isValid()) {
            return true;
        }
        //
        //  当前节点输出到缩放中
        //
        mScaleOutput=createOutputInterface();
        InputInterface input;
        FFL::sp<VideoScale> scale=new VideoScale(src,dst);
        scale->create(getOwner());
        if(!scale->connectSource(mScaleOutput,"scale", input,NULL)){
            mScaleOutput.reset();
            return false;
        }
        //
        //  缩放输入到本节点
        //
        OutputInterface sacleOutput=scale->getOutput();
        if(!connectSource(sacleOutput,"scale-composer",input,(void*)1)){
            mScaleOutput.reset();
            return false;
        }

		getPipeline()->ansyStartupNode(input.mNodeId);
        getPipeline()->ansyStartupNode(sacleOutput.mNodeId);
        return true;
    }
}
