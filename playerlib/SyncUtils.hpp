/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  SyncUtils.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/06/21
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  音画同步帮助函数
*
*/
#ifndef _SYNC_UTILS_HPP_
#define _SYNC_UTILS_HPP_

#include "SyncClock.hpp"

namespace player {
	enum OptMode {
		//
		//  正常使用修正的延迟
		//
		CORRECT_DELAY = 0,
		//
		//  相差太大，重新进行同步
		//
		RESYNC = 1,
		//
		//  丢掉当前桢
		//
		DROP_FRAME = 2,
	};
	//
	//  视频同步到master上，调整一下延迟值
	//  master:同步主时钟
	//  slave :从时钟， 视频时钟
	//  delay :原始延迟值
	//  opt :返回建议的处理模式，
	//  返回修正后的延迟值
	//  
	//
	int64_t correctVideoDelay(SyncClock* master, SyncClock* slave, int64_t delay, OptMode* opt);
	//
	//  音频同步到master上，调整一下延迟值
	//  master:同步主时钟
	//  slave :从时钟， 音频时钟
	//  delay :原始延迟值
	//  opt :返回建议的处理模式，
	//  返回修正后的延迟值
	//
	int64_t correctAudioDelay(SyncClock* master, SyncClock* slave, int64_t delay, OptMode* opt);
	//
	//  同步到master上，调整一下延迟值
	//  master:同步主时钟
	//  slave :从时钟，
	//  delay :原始延迟值
	//  opt :返回建议的处理模式，
	//  返回修正后的延迟值
	//
	int64_t correctSyncDelay(SyncClock* master, SyncClock* slave, int64_t delay, OptMode* opt);

}

#endif