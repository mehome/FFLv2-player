/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  Composer.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/01
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  合成基类
*
*/

#pragma once
#include <ref/FFL_Ref.hpp>
#include "NodeBase.hpp"

namespace player {
	class Render;
	class Composer : public NodeBase {
	public:
		Composer();
		~Composer();
		
		//
		//  获取这个节点处理的延迟
		//
		int64_t getDelayUs() { return 0; }
		//
		//  设置这个解码器的输出到那个一个渲染器中
		//
		void setOutputRender(FFL::sp<Render> composer);
		FFL::sp<Render> getRender();

		//
		//  发送消息给render
		//
		status_t postMessageDelayToRender(const FFL::sp<FFL::PipelineMessage> &msg, uint64_t delayUs);
		//
		//  获取最后一条消息的处理时间点
		//
		int64_t getLastMessageTime() const {
			return mLastPostMessageUs;
		}
	protected:
		FFL::sp<Render> mRender;	
		OutputInterface mOutputToRenderInterface;

		int64_t mLastPostMessageUs;
	};
}
