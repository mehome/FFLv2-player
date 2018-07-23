/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Player.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/03
*
*  播放器
*
*/
#include "PlayerCore.hpp"
#include <pipeline/FFL_PipelineInputHandler.hpp>
#include <pipeline/FFL_PipelineEvent.hpp>
#include <utils/FFL_Dictionary.hpp>

#include "FFMpeg.hpp"
#include "NodeBase.hpp"
#include "NodeReader.hpp"
#include "AudioComposer.hpp"
#include "VideoComposer.hpp"

#include "VideoRender.hpp"
#include "AudioRender.hpp"


#include "PlayerEvent.hpp"

#include "Decoder.hpp"
#include "reader/ReaderFactory.hpp"
#include "AudioStream.hpp"
#include "VideoStream.hpp"
#include "DeviceManager.hpp"

namespace player {
	PlayerCore::PlayerCore(FFL::PipelineEventFilter* eventFilter):mSpeed(100){
		mDeviceManager = NULL;
		mNextStreamId=0;
		
		mFileReader = NULL;
		mMasterClock = NULL;
		mVideoComposer = NULL;
		mAudioComposer = NULL;
		av_register_all();					
		mPipeline = new FFL::Pipeline();
		mPipeline->setEventFilter(eventFilter);
		setSpeed(100);
		init();
	}
	PlayerCore::~PlayerCore(){	
		if (mFileReader != NULL) {
			mFileReader->setStreamManager(NULL);
			reader::ReaderFactory::getInstance().destroyReader(mFileReader);
		}
		mPipeline = NULL;
	}
	status_t PlayerCore::init() {		
		//
		//  打开文件，读写文件节点
		//			
		mFileReader = reader::ReaderFactory::getInstance().createReader(NULL, NULL);
		mFileReader->setStreamManager(this);
		mFileReader->create(this);		
		return FFL_OK;
	}

	//
	//  没有保证同时进行多次的prepare,外部调用保证
	//
	status_t PlayerCore::syncPrepare(const char* url) {
		mUrl = url;
		mFileReader->open(mUrl.c_str());
		return FFL_OK;
	}

	status_t PlayerCore::play() {
		if (!mPipeline.isEmpty()) {		
			return mPipeline->startup();
		}
		return FFL_FAILED;
	}

	status_t PlayerCore::stop(){
		if (!mPipeline.isEmpty()){			
			mPipeline->shutdownAndWait();
			mPipeline->setEventFilter(NULL);
		}	
		return FFL_OK;
	}
	status_t PlayerCore::pause() {
		if (mMasterClock) {
			mMasterClock->pause();
		}
		mFileReader->pause();	
		
		FFL::sp<VideoRender> videoRender=getVideoRender();
		if (!videoRender.isEmpty()) {
			videoRender->pause();
		}

		return FFL_OK;
	}
	status_t PlayerCore::resume() {
		if (mMasterClock) {
			mMasterClock->resume();
		}
		mFileReader->resume();		
		return FFL_OK;
	}
	status_t PlayerCore::release() {
		if (!mPipeline.isEmpty())
		{
			mPipeline->exit();
			mPipeline.clear();
		}
		return FFL_OK;
	}
	//
	//获取，设置播放的位置,总时长
	//
	void PlayerCore::setPositionUs(int64_t pos) {
		mFileReader->seek(pos);
	}
	int64_t PlayerCore::getPositionUs() {		
		if(mMasterClock)
		   return mMasterClock->getClock();
		return mFileReader->getCurrentPosition();
	}
	int64_t PlayerCore::getDurationUs() {
		return mFileReader->getDuration();
	}	
	//
	//  设置，获取播放速度
	//
	void PlayerCore::setSpeed(uint32_t speed) {	
		mSpeed = speed;
	}
	uint32_t PlayerCore::getSpeed() {
		return mSpeed;
	}
	//
	// 获取，设置循环播放次数
	// 如果<0 : 一直循环播放
	//     =0 : 播放一次
	//     >0 : 播放num+1次
	//
	void PlayerCore::setLoop(int32_t num) {
		mFileReader->setLoopNum(num);
	}
	int32_t PlayerCore::getLoop() {
		return mFileReader->getLoopNum();
	}
	void PlayerCore::onEvent(const FFL::sp<event::PlayerEvent> event)
	{
		int32_t eventID = event->getEventId();
		switch (eventID)
		{
		case event::EVENT_PREPARED:
			//FFL_LOG_DEBUG_TAG
			event->getParam1();

			break;
		case event::EVENT_VIDEO_SIZE_CAHNGED:
		{

		}
		break;

		case event::EVENT_VIDEO_RENDER_FIRST_FRAME:

			break;
		case event::EVENT_VIDEO_RENDER_LAST_FRAME:
			//mVideoRender->stop();
			//mPipeline->shutdown();
			break;

		case event::EVENT_AUDIO_RENDER_FIRST_FRAME:

			break;

		case event::EVENT_AUDIO_RENDER_LAST_FRAME:	
			//mAudioRender->stop();
			//mPipeline->shutdown();
			break;
		default:
			break;
		}
	}
	FFL::sp<VideoRender> PlayerCore::getVideoRender(){
		FFL::sp<VideoDevice> videoDevice = mDeviceManager->getVideoDisplay(NULL);
		if (videoDevice.isEmpty()) {
			return videoDevice->getRender(NULL);
		}

		return  NULL;
	}
	FFL::sp<AudioRender> PlayerCore::getAudioRender(){
		FFL::sp<AudioDevice> audioDevice = mDeviceManager->getAudioDisplay(NULL);
		if (audioDevice.isEmpty()) {
			return audioDevice->getRender(NULL);
		}

		return  NULL;
	}
	void PlayerCore::registerNode( FFL::sp<NodeBase> node) {
		FFL::CMutex::Autolock l(mNodeLock);
		mNodeList.push_back(node);
	}
	void PlayerCore::unRegisterNode( FFL::sp<NodeBase> node) {
		FFL::CMutex::Autolock l(mNodeLock);
		for (FFL::List<  FFL::sp<NodeBase> >::iterator it = mNodeList.begin();
			it != mNodeList.end(); it++) {
			if (it->get() == node.get()) {
				mNodeList.erase(it);
				break;
			}
		}
	}

	//  IStreamManager
	//
	//  添加reader下的一个stream到管理中，返回当前这个stream的id
	//
	int32_t  PlayerCore::addStream(reader::ReaderBase* reader,StreamPtr stream) {
		if (stream.isEmpty()) {
			//
			// 最后一条
			//
			int count = 0;
			{
				FFL::CMutex::Autolock l(mStreamLock);
				count = mStreamVec.size();
			}

			if (mMasterClock == NULL) {
				mMasterClock = getStream(0)->getSyncClock();
			}
			event::postPlayerEvent(this, event::EVENT_PREPARED, count>0, 0);
			return -1;
		}		

		bool ret = false;
		switch (stream->getStreamType())
		{
		case STREAM_TYPE_VIDEO:
		{
			onAddVideoStream((VideoStream*)stream.get());
			ret = true;
		}
		break;
		case  STREAM_TYPE_AUDIO:
		{
			ret=onAddAudioStream((AudioStream*)stream.get());
		}
		break;
		default:
			onAddOtherStream(stream);
			ret = false;
			break;
		};

		if (ret) {
			stream->mStreamID = mNextStreamId++;
			FFL::CMutex::Autolock l(mStreamLock);
			mStreamVec.push_back(stream);
			return stream->mStreamID;
		}		
		return -1;
	}
	//
	//  添加reader下的所有stream到管理中，返回成功添加了几个流
	//  
	//
	uint32_t PlayerCore::addStreamVec(reader::ReaderBase* reader, FFL::Vector < StreamPtr > streamVec) {
		FFL_ASSERT(0);
		return 0;
	}
	//
	//  根据流id获取一个流实例
	//
	StreamPtr PlayerCore::getStream(int32_t id) {
		FFL::CMutex::Autolock l(mStreamLock);
		for (uint32_t i = 0; i < mStreamVec.size(); i++) {
			if (mStreamVec[i]->getIndex() == id) {
				return mStreamVec[i];
			}
		}
		return NULL;
	}
	//
	//  获取这个reader下的所有流
	//
	void PlayerCore::getStreamVec(reader::ReaderBase* reader, FFL::Vector < StreamPtr >& streamVec) {
		FFL_ASSERT(0);
	}
	bool PlayerCore::onAddVideoStream(FFL::sp<VideoStream> stream) {		
		FFL::sp<Decoder> decoder = stream->createDecoder(this);
		if (decoder.isEmpty()) {
			FFL_LOG_WARNING("Failed to create video decoder.");
			return false;
		}

		FFL::sp<VideoDevice> videoDevice = mDeviceManager->getVideoDisplay(stream);
		if (videoDevice.isEmpty()) {			
			//  如果视频设备未打开，则打开视频输出设备
			//
			videoDevice = mDeviceManager->openVideoDisplay(mDeviceManager->mSurfaceHandle);
			if (videoDevice.isEmpty()) {
				FFL_LOG_WARNING("Failed to create video device.");
				return false;
			}			
		}
		//
		// 视频处理节点
		//
		if (!mVideoComposer) {
			mVideoComposer = new VideoComposer();			
			mVideoComposer->create(this);
		}	
		//
		//  通过视频设备创建视频渲染节点
		//
		FFL::sp<VideoRender> render = videoDevice->getRender(NULL);
		if (!render->isCreated()) {
			render->create(this);
			render->setSpeed(getSpeed());
			mVideoComposer->setOutputRender(render);
		}
		//
		//  解码器解码出来的数据输出到composer中
		//	
		decoder->setOutputComposer(mVideoComposer);

		fetchVideoMetaData(stream);
		return  true;
	}
	bool PlayerCore::onAddAudioStream(FFL::sp<AudioStream> stream) {		
		FFL::sp<Decoder> decoder = stream->createDecoder(this);
		if (decoder.isEmpty()) {
			FFL_LOG_WARNING("Failed to create audio decoder.");
			return false;
		}

		//
		// 获取音频设备
		//
		FFL::sp<AudioDevice> audioDevice= mDeviceManager->getAudioDisplay(stream);
		if (audioDevice.isEmpty()) {
			AudioFormat fmt;
			stream->getFormat(fmt);			//
			//  如果音频设备未打开，则打开音频输出设备
			//
			audioDevice = mDeviceManager->openAudioDisplay(fmt);
			if (audioDevice.isEmpty()) {
				FFL_LOG_WARNING("Failed to create audio device.");
				return false;
			}
		}

		//
		//  通过音频设备创建音频渲染节点
		//
		FFL::sp<AudioRender> render = audioDevice->getRender(NULL);
		if (!render.isEmpty() && !render->isCreated()) {			
			stream->getTimebase(mAudioTb);			
			render->setSpeed(getSpeed());
			render->create(this);
		}
		//
		// 音频处理后的数据输出到render中
		//
		if (!mAudioComposer) {
			mAudioComposer = new AudioComposer();			
			mAudioComposer->setOutputFormat(audioDevice->getOpenFormat());
			mAudioComposer->create(this);
			mAudioComposer->setOutputRender(render);
		}
		//
		//  解码器解码出来的数据输出到composer中
		//		
		decoder->setOutputComposer(mAudioComposer);
		
		//  设置这个流为同步的主时钟
		//
		mMasterClock = stream->getSyncClock();	
		return true;
	}
	bool PlayerCore::onAddOtherStream(StreamPtr stream) {
		return false;
	}
	//
	// 获取视频的metadata
	//
	void PlayerCore::fetchVideoMetaData(FFL::sp<VideoStream> stream) {		
		uint32_t width=0;
		uint32_t height = 0;
		stream->getSize(width,height);		
		event::postPlayerEvent(this,event::EVENT_VIDEO_SIZE_CAHNGED,
			FFL_MAKE_INT64(width, height),0);
		//stream->getDictionary();
	}
}
