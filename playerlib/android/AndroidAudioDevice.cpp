/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  AndroidAudioDevice.cpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/28 
*  https://github.com/zhenfei2016/FFL-v2.git
*  android下播放音频的封装
*
*/
#include <utils/FFL_Handler.hpp>
#include <pipeline/FFL_PipelineEvent.hpp>
#include <pipeline/FFL_PipelineEventCallback.hpp>
#include "AndroidAudioDevice.hpp"
#include "JavaAudiotrack.hpp"
#include "jnihelper/JavaThread.hpp"

namespace android {
	//
	//  默认支持的采样格式
	//
	player::AudioFormat gDefaultFormat;

	class AudioTrackThread: public  FFL::PipelineEventLooper {
	public:
		AudioTrackThread(AndroidAudioDevice* dev):mJniEnv(NULL) {
            mDev = dev;
            mAudioThrack = NULL;
        }
		~AudioTrackThread(){
		}
		void openAudioTrack(player::AudioFormat& fmt){
			if(!mJniEnv){
				getJavaVM()->AttachCurrentThread(&mJniEnv,0);
			}
			jint streamType=AUDIO_TRACK_STREAM_MUSIC;
			jint sampleRateInHz=fmt.mFreq;
			jint channelConfig=AUDIOTRACK_CHANNEL_OUT_STEREO;
			jint audioFormat=AUDIOTRACK_ENCODING_PCM_16BIT;
			jint bufferSizeInBytes=  getJavaAudioTrackClass()->
					getMinBufferSize(mJniEnv,sampleRateInHz,channelConfig,audioFormat);
			jint mode=AUDIOTRACK_MODE_STREAM;
			mAudioThrack= getJavaAudioTrackClass()->allocAudioTrack(*mJniEnv,streamType,
			         sampleRateInHz,channelConfig,audioFormat,bufferSizeInBytes,mode);
			mAudioThrack->play();
		}

		void closeAudioTrack(){
			if(!mJniEnv){
				return;
			}

          //  unregisterIdleHandler();
			mAudioThrack->stop();
			mAudioThrack->release();
			getJavaAudioTrackClass()->freeAudioTrack(mAudioThrack);
			mAudioThrack=NULL;

			mJniEnv=NULL;
			getJavaVM()->DetachCurrentThread();
		}
 		void write(){
			if(!mJniEnv){
				return;
			}

            int32_t len=mDev->mDataCache.read(4096,&mSwapBuffer);
			if(len>0){
				mAudioThrack->write(mSwapBuffer.data(),len);
			}
		}

		JNIEnv* mJniEnv;
		AndroidAudioDevice* mDev;
		FFL::ByteBuffer mSwapBuffer;
        JavaAudioTrack* mAudioThrack;
	};

	AndroidAudioDevice::AndroidAudioDevice():mCacheUs(1000*500){
		gDefaultFormat.mChannelNum=2;
		gDefaultFormat.mFreq=48000;
		gDefaultFormat.mFormat=AV_SAMPLE_FMT_S16;
		mAudioTrackThread=new AudioTrackThread(this);
		mAudioTrackThread->start();

		mEventOpen= new FFL::PipelineEvent(
				 new FFL::ClassMethodCallback<AndroidAudioDevice>(this, &AndroidAudioDevice::handleOpenAudioTrack));

		mEventClose= new FFL::PipelineEvent(
				new FFL::ClassMethodCallback<AndroidAudioDevice>(this, &AndroidAudioDevice::handleCloseAudioTrack));

		mEventWrite= new FFL::PipelineEvent(
				new FFL::ClassMethodCallback<AndroidAudioDevice>(this, &AndroidAudioDevice::handleWriteAudioTrack));
	}
	AndroidAudioDevice::~AndroidAudioDevice(){
		mAudioTrackThread->stop();
	}

	//
	//  获取支持的格式
	//  wanted: 如果为nUll则返回所有支持的格式
	//           非null 返回跟他匹配的 
	//  fmtList: 返回支持的格式list
	//
	void AndroidAudioDevice::getSupportFormat(const player::AudioFormat* wanted, FFL::List<player::AudioFormat>& fmtList) {
		fmtList.push_back(gDefaultFormat);
	}
	bool AndroidAudioDevice::isSupportFormat(const player::AudioFormat* wanted) {
		if(wanted->equal(gDefaultFormat)){
			return true;
		}
		return false;
	}
	//
	// 设置缓冲多长时间的数据
	//
	void AndroidAudioDevice::setCacheUs(int64_t us) {
		mCacheUs=us;
	}
	//
	//  打开关闭音频设备
	//
	bool AndroidAudioDevice::onOpen(const player::AudioFormat& wanted, int32_t sampleNum, player::AudioFormat& obtained) {
		mCurrentFormat=obtained=gDefaultFormat;
		//
		// 计算每秒消耗多少字节,申请内存用于缓冲音频数据
		//
		uint32_t perSampleSize = obtained.getPerFrameSize();
		int64_t bytesPerSec = obtained.mFreq * perSampleSize;

		uint32_t bufSize =(uint32_t) (bytesPerSec * ((double)mCacheUs / (1000 * 1000)));
		bufSize = FFL_MAX(sampleNum * perSampleSize* 4, bufSize);
		mDataCache.setSize(bufSize);
		mDataCache.startup();
		mAudioTrackThread->postEvent(mEventOpen,0);

		return true;
	}
	void AndroidAudioDevice::onClose() {
		mDataCache.shutdown();
		mAudioTrackThread->postEvent(mEventClose,0);

	}
	//
	//获取当前打开的格式，如果未打开则返回null
	//
	player::AudioFormat* AndroidAudioDevice::getOpenFormat() {
		return &gDefaultFormat;
	}
	//
	//  写一帧数据
	//
	bool AndroidAudioDevice::writeFrame(player::AudioSample* samples) {
		if(mDataCache.writeFrame(samples)){
			FFL::sp<FFL::PipelineEvent> event= new FFL::PipelineEvent(
					new FFL::ClassMethodCallback<AndroidAudioDevice>(this, &AndroidAudioDevice::handleWriteAudioTrack));
			mAudioTrackThread->postEvent(event,0);
		}
        return true;
	}
	//
	//  获取播放设备硬件的延迟
	//
	int64_t AndroidAudioDevice::getDeviceDelayUs() {
		return 0;
	}

	//
	// 获取缓冲的数据量，还没有播放
	//
	int64_t AndroidAudioDevice::getCacheBytes() {
		return mDataCache.getCacheBytes();
	}
	int64_t AndroidAudioDevice::getCacheDelay(FFL::TimeBase& base) {
		return 0;
	}
	//
	//  清空缓冲的数据
	//
	int64_t AndroidAudioDevice::clearCache() {
		return mDataCache.clearCache();
	}
	//
	// 获取播放中的音频的pts
	//
	int64_t AndroidAudioDevice::getRenderingPts() {
		return mDataCache.getRenderingPts();
	}


	void AndroidAudioDevice::handleOpenAudioTrack(const FFL::sp<FFL::PipelineEvent>& event){
		mAudioTrackThread->openAudioTrack(mCurrentFormat);
	}
	void AndroidAudioDevice::handleCloseAudioTrack(const FFL::sp<FFL::PipelineEvent>& event){
		mAudioTrackThread->closeAudioTrack();
	}
	void AndroidAudioDevice::handleWriteAudioTrack(const FFL::sp<FFL::PipelineEvent>& event){
		mAudioTrackThread->write();
	}
}