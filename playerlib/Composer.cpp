/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  Composer.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/01
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  合成基类
*
*/


#include "Composer.hpp"
#include "Render.hpp"
#include <pipeline/FFL_PipelineAsyncConnectorFixedsize.hpp>

namespace player {
	Composer::Composer():mLastPostMessageUs(0){

	}
	Composer::~Composer() {

	}
	//
	//  设置这个合成器的输出到那个一个渲染器中
	//
	void Composer::setOutputRender(FFL::sp<Render> render) {
		if (mRender.get() == render.get()) {
			return;
		}

		mRender = render;
		if (render.isEmpty())
		{
			FFL_LOG_WARNING("Composer::setOutputRender render=null");
			mOutputToRenderInterface.mId = FFL::Pipeline_INVALID_Id;
			return;
		}
		if (!mOutputToRenderInterface.isValid())
			mOutputToRenderInterface = createOutputInterface();

		InputInterface input;
        mRender->connectSource(mOutputToRenderInterface, mRender->getName(), input,NULL);
	}
	FFL::sp<Render> Composer::getRender() {
		return mRender;
	}

	//
	//  发送消息给render
	//
	status_t Composer::postMessageDelayToRender(const FFL::sp<FFL::PipelineMessage> &msg, uint64_t delayUs) {
		mLastPostMessageUs = FFL_getNowUs() + delayUs;
		return postMessageDelay(mOutputToRenderInterface.mId, msg, delayUs);
	}
}
