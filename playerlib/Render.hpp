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
#pragma once
#include "NodeBase.hpp"
namespace player {	
	class Render  : public NodeBase{
	public:
		Render();
		~Render();	

		virtual void pause();
		virtual void resume();

		//
		//  获取这个节点处理的延迟
		//
		int64_t getDelayUs() { return 0; }	

		//
		//  获取，设置渲染速度
		//
		void setSpeed(uint32_t speed);
		uint32_t getSpeed();
		//
		//  获取渲染时钟，可以改变时钟速度
		//
		virtual FFL::sp<FFL::Clock> getRenderClock()=0;
	protected:
		//
		//  更新当前绘制tm时间戳的帧
		//
		void updateRenderTimestamp(int64_t tm,int32_t streamId);
	protected:
		//
		//   外部setDataInput时候调用此函数，创建对应conn
		//
		virtual FFL::sp<FFL::PipelineConnector > onCreateConnector(const OutputInterface& output,
			const InputInterface& input,void* userdata)=0;
	
	private:
		uint32_t mSpeed;
	};
}
