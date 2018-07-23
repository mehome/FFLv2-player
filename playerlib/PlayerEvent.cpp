/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PlayerEvent.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/11
*
*  播放器事件
*
*/
#include "PlayerEvent.hpp"
#include "NodeBase.hpp"
#include "PlayerCore.hpp"

namespace event{
	void postPlayerEvent(FFL::sp<player::NodeBase> node, EventId eventid) {
		node->postEvent(new PlayerEvent(eventid,0,0));
	}

	void postPlayerEvent(FFL::sp<player::NodeBase> node, const FFL::sp<PlayerEvent>& event) {
		node->postEvent(event);
	}

	//
	//  发送播放器事件
	//
	void postPlayerEvent(player::PlayerCore* core, EventId eventid, int64_t param1, int64_t param2) {
		core->getPipeline()->postEvent(new PlayerEvent(eventid, param1, param2));
	}

}
