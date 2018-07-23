/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  VideoComposer.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/05/19
*
*  视频合成
*
*/
#ifndef _VIDEO_COMPOSER_HPP_
#define _VIDEO_COMPOSER_HPP_

#include "Composer.hpp"
#include "VideoTexture.hpp"
#include "reader/Stream.hpp"
#include "Statistic.hpp"
#include "SyncUtils.hpp"
#include <pipeline/FFL_PipelineAsyncConnector.hpp>

namespace player {	
	class TimestampExtrapolator;
	class VideoComposer : public Composer {
	public:
		VideoComposer();
		~VideoComposer();

		
	protected:
		void onCreate();
		//
		//   外部setDataInput时候调用此函数，创建对应conn
		//
		virtual FFL::sp<FFL::PipelineConnector > onCreateConnector(
			const OutputInterface& output,
			const InputInterface& input,void* userdata);

		//
		// 处理接收到的消息，如果返回false表示这个消息没有处理，
		// 返回true表示这个消息处理了，消息处理结束后必须msg->consume();
		//
		virtual bool handleReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);

	private:
		//
		//  计算多长时间后播放这一桢
		//
		int64_t getDelay(VideoTexture* texture,OptMode& mode);
		//
		//  开始处理收到的纹理数据
		//
		void handleTexture(const FFL::sp<FFL::PipelineMessage>& msg,VideoTexture* texture);
		//
		//  接收到eof消息
		//
		void handleEOF(const FFL::sp<FFL::PipelineMessage>& eof);
    private:
        //
        // 进行缩放，从src格式转到dst格式,异步操作
        //
        void scaleVideo(const FFL::sp<FFL::PipelineMessage>& msg,VideoTexture* texture,VideoFormat* src,VideoFormat* dst);
        //
        // 创建缩放节点
        //
        bool createScale(int32_t streamId,VideoFormat* src,VideoFormat* dst);
	private:
		FFL::TimeBase mTb;
		TimestampExtrapolator* mTimestampExtrapolator;

        FFL::sp<FFL::PipelineAsyncConnector> mConnector;

		
        OutputInterface mScaleOutput;

	protected:
		IStatisticVideoRender* mStatistic;
	};
}

#endif