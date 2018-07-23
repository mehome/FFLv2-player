/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  Decoder.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/10
*
*  解码基类
*
*/
#include "Decoder.hpp"
#include "Composer.hpp"


namespace player {
	Decoder::Decoder(){
	}
	Decoder::~Decoder() {
	}	
	//
	//  获取这个节点处理的延迟
	//
	int64_t Decoder::getDelayUs() {
		return 0;
	}
	//
	//  设置这个解码器的输出到那个一个合成器中
	//
	void Decoder::setOutputComposer(FFL::sp<Composer> composer) {
		mComposer = composer;
		if (!mComposer.isEmpty()) {
			mFrameOutput = createOutputInterface();
			InputInterface input;
            FFL::String name;
            getName(name);
			mComposer->connectSource(mFrameOutput, mComposer->getName(),input,0);
		}
	}
	
	FFL::sp<Composer>  Decoder::getComposer() {
		return mComposer;
	}



}
