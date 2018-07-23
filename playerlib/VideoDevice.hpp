#ifndef _VIDEO_DEVICE_HPP_
#define _VIDEO_DEVICE_HPP_

#include <ref/FFL_Ref.hpp>
#include "VideoTexture.hpp"
#include "VideoSurface.hpp"

namespace player {
	class VideoRender;
	class VideoSurface;
	class VideoDevice : public FFL::RefBase{
	public:		
		VideoDevice();
		virtual ~VideoDevice();
		//
		//  获取支持的格式
		//  wanted: 如果为nUll则返回所有支持的格式
		//           非null 返回跟他匹配的
		//  fmtList: 返回支持的格式list
		//
		virtual void getSupportFormat(const player::VideoFormat* wanted,FFL::List<player::VideoFormat>& fmtList);
		virtual bool getOptimalFormat(const player::VideoFormat* wanted,player::VideoFormat* optinal);
		virtual bool isSupportFormat(const player::VideoFormat* wanted);
		//
		// 获取绘制窗口
		//		
		virtual FFL::sp<VideoSurface> getSurface()=0;
		//
		// 更新绘制的目标窗口
		//
		virtual void setSurface(SurfaceHandle surface)=0;
		//
		//  打开关闭视频设备
		//
		bool open(SurfaceHandle surface,int32_t widht,int32_t height) ;
		void close() ;
		virtual bool onOpen(SurfaceHandle wnd, int32_t widht, int32_t height)=0;
		virtual void onClose()=0;
		
		//
		//  清空cache的数据
		//
		virtual void clearCache()=0;
		//
		//  写一帧数据
		//		
		virtual bool showTexture(VideoTexture* texture)=0;
		//
		// 获取render
		//
		virtual FFL::sp<VideoRender> getRender(void* userdata) ;

		virtual void resetRender();
	private:
		FFL::sp<VideoRender> mVideoRender;
	};
}
#endif
