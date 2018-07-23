/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  PlayerCore.hpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/03/03 
 *  
 *  播放器
 *
*/
#pragma once
#include <pipeline/FFL_Pipeline.hpp>
#include <utils/FFL_Utils.hpp>
#include "PlayerEvent.hpp"
#include "VideoSurface.hpp"
#include "AudioDevice.hpp"
#include "VideoDevice.hpp"
#include "Statistic.hpp"
#include "DeviceFactory.hpp"
#include "DeviceManager.hpp"
#include "SyncClock.hpp"
#include "reader/Stream.hpp"
#include "reader/ReaderBase.hpp"
#include "reader/ReaderStreamManager.hpp"

namespace player {
	class NodeBase;
	class NodeReader;

	class VideoStream;
	class AudioStream;

	class VideoComposer;
	class AudioComposer;

	class VideoRender;
	class AudioRender;

	class TimestampExtrapolator;

	class SDL2Module;



	class PlayerCore : public reader::ReaderStreamManager {
		friend class NodeBase;
		friend class FFLPlayer;
	public:
		PlayerCore(FFL::PipelineEventFilter* eventFilter);
		virtual ~PlayerCore();
	protected:
		status_t init();
	public:
		//
		//  开始播放，停止  等等系列控制函数
		//  调用逻辑
		//  
		//
		status_t syncPrepare(const char* url);
		status_t play();
		status_t stop();
		status_t pause();
		status_t resume();
		status_t release();
	public:
		//
		//获取，设置播放的位置,总时长
		//
		void setPositionUs(int64_t pos);
		int64_t getPositionUs();
		int64_t getDurationUs();

		//
		//  设置，获取播放速度
		//
		void setSpeed(uint32_t speed);
		uint32_t getSpeed();

		//
		// 获取，设置循环播放次数
		// 如果<0 : 一直循环播放
		//     =0 : 播放一次
		//     >0 : 播放num+1次
		//
		void setLoop(int32_t num);
		int32_t getLoop();
	protected:		
		void onEvent(const FFL::sp<event::PlayerEvent> event);		
	public:
		//
		//  获取主线,所有的节点都是由他管理的
		//
		FFL::sp<FFL::Pipeline> getPipeline() {
			return mPipeline;
		}
		//
		//  获取同步的主时钟
		//
		SyncClock* getMasterClock() const {
			return mMasterClock;
		};

		FFL::sp<VideoRender> getVideoRender();
		FFL::sp<AudioRender> getAudioRender();
	private:
		uint32_t mSpeed;
		FFL::sp<FFL::Pipeline> mPipeline;		
		SyncClock*  mMasterClock;
	public:
		//////////////////////////////////////////////////////////////////////////////
		//   音视频流管理
		//////////////////////////////////////////////////////////////////////////////
		//  ReaderStreamManager
		//
		//  添加reader下的一个stream到管理中，返回当前这个stream的id
		//
		virtual int32_t  addStream(reader::ReaderBase* reader, StreamPtr stream) ;
		//
		//  添加reader下的所有stream到管理中，返回成功添加了几个流
		//  
		//
		virtual uint32_t addStreamVec(reader::ReaderBase* reader, FFL::Vector < StreamPtr > streamVec) ;
		//
		//  根据流id获取一个流实例
		//
		virtual StreamPtr getStream(int32_t id);
		//
		//  获取这个reader下的所有流
		//
		virtual void getStreamVec(reader::ReaderBase* reader, FFL::Vector < StreamPtr >& streamVec) ;
		//
		// 获取流管理器
		//
		reader::ReaderStreamManager* getStreamMgr() {
			return this;
		}
	private:
		FFL::CMutex mStreamLock;
		uint32_t mNextStreamId;
		FFL::Vector< StreamPtr >  mStreamVec;
		//
		//  添加一条音视频，数据流，返回是否已经接受了这个流
		//
		bool onAddVideoStream(FFL::sp<VideoStream> stream);
		bool onAddAudioStream(FFL::sp<AudioStream> stream);
		bool onAddOtherStream(StreamPtr stream);
		//
		// 获取视频的metadata
		//
		void fetchVideoMetaData(FFL::sp<VideoStream> stream);
	public:
		//////////////////////////////////////////////////////////////////////////////
		//   音视频设备创建，删除系类函数
		//////////////////////////////////////////////////////////////////////////////


	private:	
		FFL::TimeBase mAudioTb;

	protected:
		//
		//  节点管理
		//
		FFL::CMutex mNodeLock;
		FFL::List< FFL::sp<NodeBase> > mNodeList;
		void registerNode(FFL::sp<NodeBase> node);
		void unRegisterNode(FFL::sp<NodeBase> node);
	private:				
		
		
		VideoComposer* mVideoComposer;
		AudioComposer* mAudioComposer;

		
		FFL::sp<reader::ReaderBase> mFileReader;
	public:
		PlayerStatistic mStats;
	public:
		FFL::String mUrl;
	private:
		DeviceManager* mDeviceManager;
	public:
		void setDeviceManager(DeviceManager* mgr) {
			mDeviceManager = mgr;
		}
		DeviceManager* getDeviceManager() const {
			return mDeviceManager;
		}
	};
}
