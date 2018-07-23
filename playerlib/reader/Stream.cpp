/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  Stream.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/01
*
*  流信息
*
*/

#include "Stream.hpp"
#include "../SyncClock.hpp"

namespace reader {
	Stream::Stream(){
		mSyncClock = new player::SyncClock();		
	}	
	Stream::~Stream() {
		FFL_SafeFree(mSyncClock);
	}	
	//
	//  设置这个流的源输入接口
	//
	void Stream::setSource(const player::OutputInterface& input) {
		mSource = input;
	}
	player::OutputInterface Stream::getSource() {
		return mSource;
	}
	//
	//  获取同步时钟
	//
	player::SyncClock* Stream::getSyncClock() {
		return mSyncClock;
	}
	//
	// 获取这个流的一些属性
	//
	FFL::Dictionary* Stream::getDictionary() {
		return &mDictionary;
	}
}
