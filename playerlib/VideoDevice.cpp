#include "VideoDevice.hpp"
#include "VideoRender.hpp"

namespace player {
	VideoDevice::VideoDevice() {
	}
	VideoDevice::~VideoDevice(){
	}
	//
	//  获取支持的格式
	//  wanted: 如果为nUll则返回所有支持的格式
	//           非null 返回跟他匹配的
	//  fmtList: 返回支持的格式list
	//
	void VideoDevice::getSupportFormat(const player::VideoFormat* wanted,FFL::List<player::VideoFormat>& fmtList){

	}
	bool VideoDevice::getOptimalFormat(const player::VideoFormat* wanted,player::VideoFormat* optinal){
		return  true;
	}
	bool VideoDevice::isSupportFormat(const player::VideoFormat* wanted){
		return true;
	}
	//
	//  打开关闭视频设备
	//
	bool VideoDevice::open(SurfaceHandle surface, int32_t widht, int32_t height) {
		return onOpen(surface, widht, height);
	}
	void VideoDevice::close() {
		onClose();
		mVideoRender = NULL;
	}
	//
	// 获取render
	//
	FFL::sp<VideoRender> VideoDevice::getRender(void* userdata) {
		if (mVideoRender.isEmpty()) {
			mVideoRender = new VideoRender(this);
		}
		return mVideoRender;
	}
	void VideoDevice::resetRender() {
		mVideoRender = NULL;
	}
}
