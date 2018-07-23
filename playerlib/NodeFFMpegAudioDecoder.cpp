/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  NodeFFMpegAudioDecoder.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/15
*
*  音频解码器
*
*/

#include "MessageFFMpegPacket.hpp"
#include "NodeFFMpegAudioDecoder.hpp"
#include "FFMpegStream.hpp"
#include "PlayerCore.hpp"
#include "VideoTexture.hpp"
#include "AudioStream.hpp"
#include <pipeline/FFL_PipelineAsyncConnectorFixedsize.hpp>

namespace player {
	NodeFFMpegAudioDecoder::NodeFFMpegAudioDecoder(AudioStream* stream,AVCodecContext* ctx):
		NodeFFMpegDecoder(ctx),
		mAudioStream(stream){
		FFL_ASSERT(stream);
		setName("NodeFFMpegAudioDecoder");		
		mMessageCache = new FFL::PipelineMessageCache(MSG_FFMPEG_AUDIO_FRAME);
		mLatelyPts = 0;		
	}
	NodeFFMpegAudioDecoder::~NodeFFMpegAudioDecoder()
	{
	}
	//
	//   外部setDataInput时候调用此函数，创建对应conn
	//
	FFL::sp<FFL::PipelineConnector > NodeFFMpegAudioDecoder::onCreateConnector(
		const OutputInterface& output,
		const InputInterface& input, void* userdata) {
		FFL::PipelineAsyncConnectorFixSize* conn = new FFL::PipelineAsyncConnectorFixSize(5);
		conn->setName("audioDecoder");
		return conn;
	}
	void NodeFFMpegAudioDecoder::handleDecodedFrame(AVFrame* frame){		
		message::FFMpegAudioFrame* sample = 0;
		FFL::sp<FFL::PipelineMessage> msg = message::createMessageFromCache(mMessageCache, &sample, MSG_FFMPEG_AUDIO_FRAME);
		correctTimestamp(sample, frame);

		sample->fillAvframe(frame);
		sample->mSamples.mStreamId = mAudioStream->getIndex();		
		mAudioStream->getFormat(mFormat);		
		sample->mSamples.setAudioFormat(mFormat);		

		FFL::sp<FFL::PipelineOutput> output=getOutput(mFrameOutput.mId);
		if (!output.isEmpty()) {
			if (output->getPendingMessageSize() >= 5) {
				FFL_sleep(10);
			}
		}

		if (FFL_OK != postMessage(mFrameOutput.mId, msg)) {
			msg->consume(this);
		}	
	}
	void NodeFFMpegAudioDecoder::handleEOF(const FFL::sp<FFL::PipelineMessage>& eof) {
		postMessage(mFrameOutput.mId, eof);
	}
	//
	//  修正时间戳
	//
	void NodeFFMpegAudioDecoder::correctTimestamp(message::FFMpegAudioFrame* audioFrame, AVFrame* frame){
		//
		//  计算pts
		//
		int64_t pts = frame->pts;
		if (pts == AV_NOPTS_VALUE) {
			pts = frame->pkt_dts;
		}		
		frame->pts = pts;	
	}
}
