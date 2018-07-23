/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  AndroidVideoDevice.cpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/28 
*  https://github.com/zhenfei2016/FFL-v2.git
*  android播放音频接口
*
*/
#include "AndroidVideoDevice.hpp"
#include "render/RenderManager.hpp"
#include "VideoFormat.hpp"

namespace android {
	class ANativeSurface : public  player::VideoSurface{
	public:
        ANativeSurface(){
            mVideoFormat.mFormat=player::VideoFormat::PIXEL_FORMAT_RGBA_8888;
        }
        void setHandle(SurfaceHandle handle){
            player::VideoSurface::setHandle(handle);

			int32_t hardwareFmt=-1;
			ANativeWindow* win=handle;
			if(win) {
				hardwareFmt =ANativeWindow_getFormat(win);
				mVideoFormat.mWidht = ANativeWindow_getWidth(win);
				mVideoFormat.mHeight = ANativeWindow_getHeight(win);
			}else{
				mVideoFormat.mWidht = -1;
				mVideoFormat.mHeight = -1;
			}


			switch (hardwareFmt){
				case AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM:
				case AHARDWAREBUFFER_FORMAT_R8G8B8X8_UNORM:
					mVideoFormat.mFormat=player::VideoFormat::PIXEL_FORMAT_RGBA_8888;
					break;
				default:
					mVideoFormat.mFormat=player::VideoFormat::PIXEL_FORMAT_NONE;
			}
        }
		void setWindowSize(int32_t widht, int32_t height) {

		}
		bool getWindowSize(int32_t& width, int32_t& height){
			ANativeWindow* win=getHandle();
			if(win) {
				width = ANativeWindow_getWidth(win);
				height = ANativeWindow_getHeight(win);
				return true;
			}
			return  false;
		}
	};
	AndroidVideoDevice::AndroidVideoDevice():mIsOpened(false){
		mRenderer=NULL;
		mSurfaceHandle=NULL;
		mVideoSurface=new ANativeSurface();
		mRenderManager=new RenderManager();
		mRenderManager->registerRenders();
	}
	AndroidVideoDevice::~AndroidVideoDevice(){
		mRenderManager->unRegisterRenders();
		FFL_SafeFree(mRenderManager);
	}
//
	//  获取支持的格式
	//  wanted: 如果为nUll则返回所有支持的格式
	//           非null 返回跟他匹配的
	//  fmtList: 返回支持的格式list
	//
	void AndroidVideoDevice::getSupportFormat(const player::VideoFormat* wanted,FFL::List<player::VideoFormat>& fmtList){
		if(!mVideoSurface.isEmpty()){
			RenderInterface* render=mRenderManager->getRender(wanted,NULL);
			return render->getSupportFormat(mVideoSurface.get(),wanted,fmtList);
		}
	}
	bool AndroidVideoDevice::getOptimalFormat(const player::VideoFormat* wanted,player::VideoFormat* optinal){
        if(!mVideoSurface.isEmpty()){
			RenderInterface* render=mRenderManager->getRender(wanted,NULL);
			return render->getOptimalFormat(mVideoSurface.get(),wanted,optinal);
		}
		return false;
	}
	bool AndroidVideoDevice::isSupportFormat(const player::VideoFormat* wanted){
        if(!mVideoSurface.isEmpty()){
			RenderInterface* render=mRenderManager->getRender(wanted,NULL);
			return render->isSupportFormat(mVideoSurface.get(),wanted);
		}
		return false;
	}
	// 获取绘制窗口
	//		
	FFL::sp<player::VideoSurface> AndroidVideoDevice::getSurface() {
		return mVideoSurface;
	}
	//
	// 更新绘制的目标窗口
	//
	void AndroidVideoDevice::setSurface(SurfaceHandle surface) {
		if(mSurfaceHandle==surface){
			return;
		}

		if(surface) {
			ANativeWindow_acquire(surface);
		}

		if(mSurfaceHandle){
			ANativeWindow_release(mSurfaceHandle);
		}
		mSurfaceHandle=surface;
		mVideoSurface->setHandle(surface);
	}
	//
	//  打开关闭视频设备
	//
	bool AndroidVideoDevice::onOpen(SurfaceHandle surface, int32_t widht, int32_t height) {
		setSurface(surface);
		mIsOpened= true;
		return true;
	}
	void AndroidVideoDevice::onClose() {
		mIsOpened=false;

		mVideoSurface->setHandle(NULL);
		if(mSurfaceHandle){
			ANativeWindow_release(mSurfaceHandle);
			mSurfaceHandle=NULL;
		}


	}
	//
	//  清空cache的数据
	//
	void AndroidVideoDevice::clearCache() {

	}
	//
	//  写一帧数据
	//		
	bool AndroidVideoDevice::showTexture(player::VideoTexture* texture) {
		if(!mIsOpened){
            if(mRenderer){
                mRenderer->destroy();
                mRenderer=NULL;
                return false;
            }

			FFL_LOG_WARNING("Failed to AndroidVideoDevice::showTexture. not open");
			return  false;
		}
		if(mRenderer==NULL) {
			mRenderer = mRenderManager->getRender(texture->getVideoFormat(), NULL);
			mRenderer->create(mVideoSurface.get(),texture->getVideoFormat());
		}

		if(mRenderer) {
			mRenderer->draw(mVideoSurface.get(), texture);
		}
		return true;
	}
}