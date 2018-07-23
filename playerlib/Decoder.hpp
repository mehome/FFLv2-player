/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  Decoder.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/10
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  解码基类
*
*/
#pragma once
#include "NodeBase.hpp"
namespace player {
	class Composer;
	class Decoder : public NodeBase{
	public:
		Decoder();
		~Decoder();			
		//
		//  获取这个节点处理的延迟
		//
		int64_t getDelayUs();
		//
		//  设置这个解码器的输出到那个一个合成器中
		//
		void setOutputComposer(FFL::sp<Composer> composer);
		FFL::sp<Composer> getComposer();		
	
	protected:
		OutputInterface mFrameOutput;
		FFL::sp<Composer> mComposer;
	};
}
