/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  AudioComposer.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/07
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  声音的合成类
*
*/
#pragma once
#include "Composer.hpp"
#include "TimestampExtrapolator.hpp"
#include "MessageFFMpegFrame.hpp"
#include "AudioResample.hpp"
#include <utils/FFL_File.hpp>

namespace player {
	class AudioComposer : public Composer {
	public:
		AudioComposer();
		~AudioComposer();

		//
		//  设置输出的格式
		//
		void setOutputFormat(AudioFormat* fmt);
	protected:
		//
		//   外部setDataInput时候调用此函数，创建对应conn
		//
		FFL::sp<FFL::PipelineConnector > onCreateConnector(
			const OutputInterface& output,
			const InputInterface& input, void* userdata);

		//
		// 处理接收到的消息，如果返回false表示这个消息没有处理，
		// 返回true表示这个消息处理了，消息处理结束后必须msg->consume();
		//
		virtual bool handleReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);

		void handleSamples(const FFL::sp<FFL::PipelineMessage>& msg,AudioSample* sample);
		//
		//  接收到eof消息
		//
		void handleEOF(const FFL::sp<FFL::PipelineMessage>& eof);

		//
		//  计算多长时间后播放这一桢
		//
		int64_t getDelay(AudioSample* sample);
	protected:
		FFL::TimeBase mTb;
	
		TimestampExtrapolator* mTimestampExtrapolator;
		//
		//  音频重采样
		//
		AudioResample*  mResample;
		//
		//  合成的目标格式
		//
		AudioFormat* mDstFormat;

		FFL::File mTestFile;
	};
}
