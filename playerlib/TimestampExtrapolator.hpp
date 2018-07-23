/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  TimestampExtrapolator   
*  Created by zhufeifei(34008081@qq.com) on 2018/05/26 
*
*  音视频同步的时间戳推算器，推算这个timestamp需要延迟多长时间开始播放
*  
*  主时钟需要一直 update();
*  需要计算延迟时长的  getDelayUsRelativeNow(),返回相对现在需要延迟多长时间
*/
#ifndef _TIMESTAMPEXTRAPOLATOR_HPP_
#define _TIMESTAMPEXTRAPOLATOR_HPP_

#include <FFL.h>
#include <utils/FFL_Clock.hpp>
#include "reader/Stream.hpp"

namespace player {
	class TimestampExtrapolator {
	public:
		//
		//  clock: 依赖这一个时钟，进行时间的推算
		//  可以改变这个clock的速度，这样计算出来的延迟值就相应的改变了
		// 
		TimestampExtrapolator(const char* name);
		~TimestampExtrapolator();
		//
		// 重置
		//
		void reset();
		//
		//  设置时钟的速度
		//
		void setSpeed(uint32_t speed);
		uint32_t getSpeed() const;
		////
		////  更新这个推算器的依赖时间点，
		////
		//void update(int64_t timestamp, const FFL::TimeBase& tb);
		////
		//// 获取这个时间戳相对当前需要延迟的时长us
		////
		//int64_t getDelay(int64_t timestamp,const FFL::TimeBase& tb);
		//
		//  lastFrameDuration:返回相对上一桢需要延迟的时间，其实就是上一桢图像的duration
		//
		int64_t getDelayAndUpdate(int64_t timestamp, const FFL::TimeBase& tb,int64_t* lastFrameDuration=0);
	private:	
		struct Tick{
			Tick() {
				mTimestampClock = mWorldClock = 0;
			}

			bool isValid() { 
				return mTimestampClock != 0 && mWorldClock != 0;
			}
			//
			//  时间戳对应的时间点，通过时间戳直接计算的
			//
			int64_t mTimestampClock;
			//
			//  当前mClock对应的时间点
			//
			int64_t mWorldClock;
			int64_t mLastShowTime;
		};
		//
		//  最近的时钟滴答，由update进行更新
		//
		Tick mRecentTick;	
		uint32_t mSpeed;

		FFL::String mName;
		int64_t mFrameIndex;
	};
}

#endif
