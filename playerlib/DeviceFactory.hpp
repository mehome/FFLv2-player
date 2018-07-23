/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  DeviceFactory.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/06/22
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  设备创建接口
*/
#ifndef _AVDEVICE_CREATOR_HPP_
#define _AVDEVICE_CREATOR_HPP_

#include <ref/FFL_Ref.hpp>
namespace player {
	class VideoDevice;
	class AudioDevice;	
	class DeviceFactory {
	public:
		DeviceFactory();
		virtual ~DeviceFactory();
		//
		//  创建播放视频设备
		//
		virtual FFL::sp<VideoDevice> createVideoDevice(void* player)=0;
		//
		//  创建播放音频设备
		//
		virtual FFL::sp<AudioDevice> createAudioDevice(void* player)=0;
	};
}
#endif
