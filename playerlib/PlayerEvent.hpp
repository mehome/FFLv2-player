/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_PlayerEvent.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/11
*
*  播放器事件
*
*/
#pragma once
#include <pipeline/FFL_PipelineEventCallback.hpp>
#include <pipeline/FFL_Pipeline.hpp>

namespace player {
	class NodeBase;
	class PlayerCore;
}

namespace event{
   enum EventId{
	   //
	   //  未知的错误
	   //
	   EVENT_ERROR =1,

	   //
	   //  prepare结果，  
	   //  getParam1() = 1  :成功
	   //  getParam1() = 0  :异常，失败了
	   //
	   EVENT_PREPARED,
	   //
	   //  暂停了
	   //
	   EVENT_PAUSE,
	   //
	   //  暂停状态恢复了
	   //
	   EVENT_RESUME,
	   //
	   //  视频大小改变了
	   //
	   EVENT_VIDEO_SIZE_CAHNGED,
	   //
	   // 定位到指定位置
	   //
	   EVENT_SEEK_START,
	   EVENT_SEEK_END,
	   //
	   // 开始，结束缓冲中
	   //
	   EVENT_BUFFERING_START,
	   EVENT_BUFFERING_END,
	   //
	   //  开始渲染视频第一，最后一帧
	   //
	   EVENT_VIDEO_RENDER_FIRST_FRAME,
	   EVENT_VIDEO_RENDER_LAST_FRAME,
	   //
	   //  开始渲染音频第一，最后一帧
	   //
	   EVENT_AUDIO_RENDER_FIRST_FRAME,
	   EVENT_AUDIO_RENDER_LAST_FRAME,
   };

   class PlayerEvent : public FFL::PipelineEvent{
   public:
	   PlayerEvent(EventId eventid,int64_t param1,int64_t param2) 
		   :FFL::PipelineEvent((int32_t)eventid),
		   mParam1(param1),
		   mParam2(param2){
	   }	      

	   inline int64_t getParam1() const {
		   return mParam1;
	   }
	   inline int64_t getParam2() const {
		   return mParam2;
	   }

	   int64_t mParam1;
	   int64_t mParam2;

   };
   //
   //  发送一些播放器事件
   //   
   void postPlayerEvent(FFL::sp<player::NodeBase> node, EventId eventid);
   void postPlayerEvent(FFL::sp<player::NodeBase> node, const FFL::sp<PlayerEvent>& event);

   //
   //  发送播放器事件
   //
   void postPlayerEvent(player::PlayerCore* core, EventId eventid, int64_t param1, int64_t param2);
}
