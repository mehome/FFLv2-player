/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  AudioComposer.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/07
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  声音的合成类
*
*/

#include "AudioComposer.hpp"
#include "PlayerCore.hpp"
#include "MessageFFMpegFrame.hpp"
#include "AudioResample.hpp"
#include "FFMpeg.hpp"
#include "SyncUtils.hpp"
#include "PlayerConstant.hpp"

namespace player {
	AudioComposer::AudioComposer():
		mTimestampExtrapolator(NULL),
		mDstFormat(NULL) {
		setName("AudioComposer");
		mResample = new AudioResample();
		mTimestampExtrapolator = new TimestampExtrapolator("audio");
#ifdef SAVE_PCM
		FFL::String file;
		file = "e://1.pcm";
		mTestFile.create(file);
#endif
	}

	AudioComposer::~AudioComposer() {
#ifdef SAVE_PCM
		mTestFile.close();
#endif
		FFL_SafeFree(mResample);
		FFL_SafeFree(mDstFormat);
		FFL_SafeFree(mTimestampExtrapolator);
	}
	//
	//  设置输出的格式
	//
	void AudioComposer::setOutputFormat(AudioFormat* fmt) {
		FFL_SafeFree(mDstFormat);
		if (fmt) {
			mDstFormat = new AudioFormat();
			*mDstFormat = *fmt;
		}		
	}

	//
	//   外部setDataInput时候调用此函数，创建对应conn
	//
	FFL::sp<FFL::PipelineConnector > AudioComposer::onCreateConnector(
		const OutputInterface& output,
		const InputInterface& input, void* userdata) {
		FFL::PipelineAsyncConnector * conn=new FFL::PipelineAsyncConnector();
		conn->setName("audioComposer");
		return conn;
	}

	//
	// 处理接收到的消息，如果返回false表示这个消息没有处理，
	// 返回true表示这个消息处理了，消息处理结束后必须msg->consume();
	//
	bool AudioComposer::handleReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata){
		if (msg.isEmpty()) {
			return true;
		}
		
		switch (msg->getType())
		{
		case MSG_FFMPEG_AUDIO_FRAME:
		{
			message::FFMpegAudioFrame* frame = (message::FFMpegAudioFrame*)msg->getPayload();
			handleSamples(msg, &(frame->mSamples));
		}
			break;
		case MSG_CONTROL_READER_SEEK:
		{		
			int64_t delayUs = 0;
			int64_t flag = msg->getParam2();
			if ((flag & 0x01) == 1) {
				clearMessage(mOutputToRenderInterface.mId);
				delayUs = -1;
			}

			mTimestampExtrapolator->reset();

			int64_t now=FFL_getNowUs();
			if (now > getLastMessageTime()) {
				delayUs = now - getLastMessageTime();
			}
			if (FFL_OK != postMessageDelayToRender( msg, delayUs)) {
				msg->consume(this);
			}
			break;
		}
		case MSG_CONTROL_READER_EOF:
			handleEOF(msg);
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
	int64_t AudioComposer::getDelay(AudioSample* sample) {
		StreamPtr stream = getOwner()->getStream(sample->mStreamId);
		stream->getTimebase(mTb);

		uint32_t speed = getOwner()->getSpeed();
		mTimestampExtrapolator->setSpeed(speed);

		int64_t delay = mTimestampExtrapolator->getDelayAndUpdate(sample->mPts, mTb);
		if (delay < 0) {
			delay += 0;
		}		
		delay = FFL_MAX(0, delay);


		OptMode mode;
		int64_t correctDelay = correctAudioDelay(getOwner()->getMasterClock(),
			stream->getSyncClock(),
			delay, &mode);

		FFL_LOG_DEBUG_TAG(TAG_TIMESTAMP,
			"AudioComposer pts=%" lld64 " delay=%" lld64 " correctDelay=%" lld64,
			sample->mPts, delay, correctDelay);

		return correctDelay;
	}

	int gFrameCOunt = 0;
	void AudioComposer::handleSamples(const FFL::sp<FFL::PipelineMessage>& msg, AudioSample* sample) {
		if (!mDstFormat) {
			FFL_LOG_WARNING("Faild to AudioComposer::handleSamples (dstFormat is null)");
			msg->consume(this);
			return;
		}

		AudioFormat fmt;
		sample->getAudioFormat(fmt);
		if (!mDstFormat->equal(fmt)) {
			//
			//  重采样
			//
			AudioSample targetSample;
			targetSample.setAudioFormat(*mDstFormat);			
			mResample->resample(sample, &targetSample, getOwner()->getSpeed());
			sample->moveData(targetSample);
			
#ifdef SAVE_PCM
            size_t writed = 0;
			mTestFile.write((*sample->mData), sample->mLinesize, &writed);
#endif 		
			if (gFrameCOunt++ > 100) {
				
			}
		}else {
			//
			//  不需要重采样
			//		
		}

		int64_t delay=getDelay(sample);
		if (delay > 100 * 1000) {
			delay /= 2;
		}
		
		//
		// 发送到这个输出接口上
		//		
		if (FFL_OK != postMessageDelayToRender(msg, delay)) {
			msg->consume(this);
		}			
	}
	//
	//  接收到eof消息
	//
	void AudioComposer::handleEOF(const FFL::sp<FFL::PipelineMessage>& eof) {
		postMessageDelayToRender(eof,0);
	}
}
