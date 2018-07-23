/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  MessageBase.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/11
*
*  消息的基类
*
*/

#include "MessageBase.hpp"

//#define ENABLE_MESSAGE_LOG
namespace message {
	MessageBase::MessageBase()
	{
//#if defined(ENABLE_MESSAGE_LOG)
		FFL_LOG_INFO("MessageBase:MessageBase %p", this);
//#endif
	}
	MessageBase::~MessageBase()
	{
//#if defined(ENABLE_MESSAGE_LOG)
		FFL_LOG_INFO("MessageBase:~MessageBase(%d:%s) %p",
                 getType(),
                 getMsgName(getType()), this);
//#endif
	}
    
    int32_t MessageBase::getType(){
        return mPayloadType;
    }
    void MessageBase::setType(int32_t type){
        mPayloadType=type;
    }
	//
	//  已经处理完成了，可以回收了		
	//
	void MessageBase::consume() {
#if defined(ENABLE_MESSAGE_LOG)
		FFL_LOG_INFO("MessageBase:consume(%d) %p",getType(), this);		
#endif
	}
	
}
