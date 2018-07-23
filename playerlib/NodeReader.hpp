/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  NodeReader.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/03
*
*  视频文件读写，解复用的
*
*/

#pragma once
#include "NodeBase.hpp"
#include "IReader.hpp"

namespace player {
	class NodeReader : public NodeBase, public IReader {
	public:
		NodeReader();
		~NodeReader();
	protected:
		//
		//  成功创建了node
		//
		virtual void onCreate();
		//
		//  成功删除了node
		//
		virtual void onDestroy();
	public:
		//
		//  打开
		//
		void play(const char* url);
		//
		//  暂停读，但是不关闭
		//
		void pause();
		//
		//  恢复读
		//
		void resume();
		//
		// 设置开始读位置
		//
		void seek(int64_t pos);
		//
		// 关闭
		//
		void close();
	public:
		//
		// 获取播放时长
		//
		int64_t getDurationUs() ;
		//
		// 获取当前的播放位置
		//
		int64_t getCurrentUs() ;

	private:
		//
		// 控制输入，数据输入接口
		//
		InputInterface mControlInput;
		InputInterface mDataInput;
		//
		// 数据处理完成后的输出接口
		//
		OutputInterface mDataOutput;
		//
		//
		//
		IReader* mImpl;
	};
}
