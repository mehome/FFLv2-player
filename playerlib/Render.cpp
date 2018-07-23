/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  Render.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/10
*
*  渲染基类
*
*/

#include "Render.hpp"
#include <pipeline/FFL_PipelineAsyncConnectorFixedsize.hpp>

namespace player {	
	Render::Render()  {
	}
	Render::~Render() {
	}

	void Render::pause() {

	}
	void Render::resume() {

	}

	//
	//  获取，设置渲染速度
	//
	void Render::setSpeed(uint32_t speed) {
		mSpeed = speed;

		FFL::sp<FFL::Clock> clock = getRenderClock();
		if (!clock.isEmpty()) {
			clock->setSpeed(mSpeed);
		}
	}
	uint32_t Render::getSpeed() {
		return mSpeed;
	}

	
}