
/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  SyncUtils.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/06/21
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  音画同步帮助函数
*
*/
#include <FFL.h>
#include <utils/FFL_Clock.hpp>
#include "reader/Stream.hpp"
#include "SyncUtils.hpp"
#include "PlayerConstant.hpp"

namespace player {
	//
	//  重新同步的阈值us，大于30s后进行重新同步
	//
#define SYNC_RESYNC_THRESHOLD_US 30000000
	//
	//  需要进行同步的阈值，40ms， 25帧的情况下时间值
	//  如果与主时钟相差大于40ms则需要进行同步
	//  >SYNC_MIN_THRESHOLD_US 不需要同步
	//  >SYNC_MAX_THRESHOLD_US 丢帧	
	//
#define SYNC_MIN_THRESHOLD_US 40000
#define SYNC_MAX_THRESHOLD_US 40000*4

	//
	//  视频同步到master上，调整一下延迟值
	//  master:同步主时钟
	//  slave :从时钟， 视频时钟
	//  delay :原始延迟值
	//  opt :返回建议的处理模式，
	//  返回修正后的延迟值
	//
	int64_t correctVideoDelay(SyncClock* master, SyncClock* slave, int64_t delay, OptMode* opt) {
		return correctSyncDelay(master,slave,delay,opt);
	}
	//
	//  音频同步到master上，调整一下延迟值
	//  master:同步主时钟
	//  slave :从时钟， 音频时钟
	//  delay :原始延迟值
	//  opt :返回建议的处理模式，
	//  返回修正后的延迟值
	//
	int64_t correctAudioDelay(SyncClock* master, SyncClock* slave, int64_t delay, OptMode* opt) {
		return delay;
	}
	//
	//  同步到master上，调整一下延迟值
	//  master:同步主时钟
	//  slave :从时钟，
	//  delay :原始延迟值
	//  opt :返回建议的处理模式，
	//  返回修正后的延迟值
	//
	int64_t correctSyncDelay(SyncClock* master, SyncClock* slave, int64_t delay, OptMode* opt) {
		if (opt) {
			*opt = CORRECT_DELAY;
		}

		if (master == NULL || slave == NULL) {
			return delay;
		}

		if (slave->getClock() == 0 || master->getClock() == 0) {
			return delay;
		}

		int64_t diff = diffClock(slave,master);
		if (diff == 0) {
			return delay;
		}

		//
		//  是否视频变快了
		//  diff>0 从时钟走的快了，需要延迟一下从时钟
		//
		bool isFast = diff > 0;
		{
			//
			//  是否需要重新同步，音视频不同步相差太大了
			//
			bool correct = false;
			if (isFast && diff >= SYNC_RESYNC_THRESHOLD_US) {
				delay = delay * 3;
				correct = true;
			}
			else if (!isFast && diff <= -SYNC_RESYNC_THRESHOLD_US) {
				delay = 0;
				correct = true;
			}

			if (correct) {
				FFL_LOG_DEBUG_TAG(TAG_TIMESTAMP,
					"sync: resync diff=%" lld64 " fast=%d",
					diff, isFast);

				if (opt)
					*opt = RESYNC;
				return delay;
			}
		}

		//
		//  不需要同步
		//
		int64_t threshold = FFL_MIN(SYNC_MIN_THRESHOLD_US, delay);
		if ((isFast && diff < threshold) ||
			(!isFast && diff > -threshold)) {
			FFL_LOG_DEBUG_TAG(TAG_TIMESTAMP,
				"sync: not adjust diff=%" lld64 " threshold=%" lld64 " fast=%d",
				diff, threshold, isFast);
			return delay;
		}


		{
			//
			//  是否丢帧检测
			//
			bool correct = false;
			if (isFast && diff > SYNC_MAX_THRESHOLD_US) {
				delay = delay*2;
				correct = true;
			}
			else if (!isFast && diff < -SYNC_MAX_THRESHOLD_US) {
				delay = 0;
				correct = true;
			}

			if (correct) {
				FFL_LOG_DEBUG_TAG(TAG_TIMESTAMP,
					"sync: dropframe diff=%" lld64 " fast=%d",
					diff,isFast);
				if (opt)
					*opt = DROP_FRAME;
				return delay;
			}
		}

		//
		//  根据不同步时长diff，调整延迟时长
		//		
		if (isFast) {
			delay = FFL_MIN(delay * 2, delay + diff);			
		}else{
			delay = FFL_MIN(delay / 2, delay + diff);
			delay = FFL_MAX(0, delay);			
		}		
		FFL_LOG_DEBUG_TAG(TAG_TIMESTAMP,
			"sync: crorect diff=%" lld64 " delay=%" lld64 " fast=%d",
			diff, delay,isFast);
		return delay;
	}
}

