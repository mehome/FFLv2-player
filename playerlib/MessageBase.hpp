/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  MessageBase.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/11
*
*  消息的基类
*
*/
#pragma once
#include <pipeline/FFL_PipelineMessage.hpp>
#include <pipeline/FFL_PipelineMessageCache.hpp>
#include "MessageTypes.hpp"

namespace message {
    class MessageBase;
    
	class MessageBase : public FFL::PipelineMessagePayload
	{
	public:
		MessageBase();
		virtual ~MessageBase();
		//
		//  获取类型
		//
        int32_t getType();
    protected:
        void setType(int32_t type);
        int32_t mPayloadType;
    public:
		//
		//  已经处理完成了，可以回收了		
		//
		virtual void consume();
	};

	//
	//  cache中创建消息
	//
	template<class PLAYLOAD>
	FFL::sp<FFL::PipelineMessage> createMessageFromCache(
		FFL::sp<FFL::PipelineMessageCache> cache,
		PLAYLOAD** payload, int32_t playloadType)
	{
		FFL::sp<FFL::PipelineMessage> msg = cache->dumpIfNullAlloc();
		PLAYLOAD* pl = NULL;
		//
		//  获取缓存
		//
		do {
			FFL::PipelineMessagePayload* payload = msg->getPayload();
			if (payload != NULL && msg->getType() == playloadType) {
				pl = (PLAYLOAD*)payload;
			}

			if (pl) break;

			//
			//  如果不存在则创建他
			//
			pl = new PLAYLOAD();
			msg->setPayload(pl);
		} while (0);

		*payload = pl;
		return msg;
	}
}
