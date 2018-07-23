/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  Player.hpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/06/20 
 *  https://github.com/zhenfei2016/FFL-v2.git
 *  
 *  播放对外接口
 *
*/
#ifndef _FFL_PALYER_HPP_
#define _FFL_PALYER_HPP_

#include <utils/FFL_Flags.hpp>
#include <thread/FFL_Mutex.hpp>
#include <utils/FFL_String.hpp>

#include <pipeline/FFL_PipelineEventFilter.hpp>
#include "PlayerInterface.hpp"
#include "VideoSurface.hpp"
#include "VideoStream.hpp"
#include "AudioStream.hpp"


namespace player {
	class PlayerCore;
	class AudioDevice;
	class VideoDevice;
	class DeviceFactory;
	class DeviceManager;

	class FFLPlayer : public IPlayer,
		 public FFL::PipelineEventFilter{
	public:
		FFLPlayer(DeviceManager* mgr=NULL);
		virtual ~FFLPlayer();

		void create(void* surface);
	public:
		//
		// FFL::PipelineEventFilter
		// 派发消息前的准备，消息派发前会先到这地方的
		// 返回false表示不要向下派发了
		//
		bool onPrepareDispatch(const FFL::sp<FFL::PipelineEvent> event);		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//   IPlayer
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//  设置播放url
		//
		virtual status_t setUrl(const char *url);
		//
		// 设置渲染到的目标
		//
		virtual status_t setSurface(void* surface);
		//
		// 设置渲染窗口的大小，只有窗口存在的情况下才可以设置大小，否则返回失败
		//
		virtual status_t setSurfaceSize(int32_t widht,int32_t height);
		//
		//  播放，暂停，停止
		//
		virtual status_t prepare();
		virtual status_t start();
		//
		//  pause:1 进行暂停， 0：恢复
		//
		virtual status_t pause(int32_t pause);
		virtual status_t stop();
		//
		//  定位到指定us处
		//  获取当前的播放位置us
		//  获取总的播放时长us
		//
		virtual status_t seekTo(int64_t us);
		virtual int64_t  getCurrentPosition();
		virtual int64_t  getDuration();
		//
		//  获取，设置播放速度，正常速度=100
		//
		virtual uint32_t getSpeed();
		virtual void setSpeed(uint32_t speed);
		//
		// 获取，设置音量
		//  0-255
		//
		virtual void setVolume(int32_t volume);
		virtual int32_t getVolume();
		//
		// 获取，设置循环播放次数
		// 如果<0 : 一直循环播放
		//     =0 : 播放一次
		//     >0 : 播放num+1次
		//
		virtual void setLoop(int32_t num);
		virtual int32_t getLoop();
		//
		// 获取，设置一些特定的选项
		//
		virtual void setOptionString(const char* name, const char* value);
		virtual void getOptionString(const char* name, char* buf, uint32_t bufSize, const char* defaultVal);
		//
		// 获取，设置一些特定的选项
		//
		virtual void setOptionInt64(const char* name, int64_t value);
		virtual void getOptionInt64(const char* name, int64_t* value, int64_t defaultVal);
		//
		//  设置播放器一系列事件的回调
		//
		virtual void setListener(IPlayerListener* listener);
	private:
		bool isPrepared() const;
		bool isStarted() const;
		bool isPaused() const;
		bool isStoped() const;
		bool isStarting() const;
		bool isPauseing() const;
		bool isStoping() const;
		bool isLooping() const;
	public:
		FFL::sp<FFL::PipelineEvent> mEventPrepare;
		//
		//  prepare 播放，暂停，停止具体实现函数
		//
		void  onPrepare(const FFL::sp<FFL::PipelineEvent>& even);
		void  onStart(const FFL::sp<FFL::PipelineEvent>& even);
		void  onPause(const FFL::sp<FFL::PipelineEvent>& even);
		void  onStop(const FFL::sp<FFL::PipelineEvent>& even);
	private:
		PlayerCore* mCore;
		//
		//  播放的url
		//
		FFL::String mUrl;
		//
		// 播放器状态回调
		//
		IPlayerListener* mListener;

		enum {
			FLAG_INIT=0,
			//
			//  prepare中 和prepare结束
			//
			FLAG_PREPAREING = 0x01,
			FLAG_PREPARED   = 0x02,
			//
			//  启动中，停止中，播放中
			//
			FLAG_STARTING =0x04,
			FLAG_STOPING = 0x08,
			FLAG_LOOPING = 0x10,
			//
			// 暂停状态
			//
			FLAG_PAUSEING =0x20,
			FLAG_PAUSED = 0x40,
		};
		FFL::CMutex mMutex;
		mutable FFL::Flags32b mFlag;	
	private:
		//
		//  管理设备创建，关闭的
		//
		DeviceManager* mDevManager;
		bool mAudoDeleteDevManager;

		//
		// 播放速度
		//
		uint32_t mSpeed;		
		//
		// 显示窗口句柄
		//
		SurfaceHandle mSurfaceHandle;		
	};
}
#endif
