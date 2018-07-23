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

#include "NodeReader.hpp"
#include "NodeFFMpegInputFile.hpp"
#include "PlayerCore.hpp"
#include <pipeline/FFL_PipelineInputHandler.hpp>

namespace player {
	NodeReader::NodeReader(){		
	}
	NodeReader::~NodeReader() {
	}
	//
	//  成功创建了node
	//
	void NodeReader::onCreate() {

	}
	//
	//  成功删除了node
	//
	void NodeReader::onDestroy(){

	}

	//
	//  打开
	//
	void NodeReader::play(const char* url) {

		if (1) {
			IStreamManager* mgr = getOwner()->getStreamMgr();
			FFL::sp<NodeFFMpegInputFile> file  = new NodeFFMpegInputFile(mgr);
			file->create(getOwner());

			mImpl = file.get();
			mImpl->play(url);
		}

	}
	//
	//  暂停读，但是不关闭
	//
	void NodeReader::pause() {
		mImpl->pause();
	}
	//
	//  恢复读
	//
	void NodeReader::resume() {
		mImpl->resume();
	}
	//
	// 设置开始读位置
	//
	void NodeReader::seek(int64_t pos) {
		mImpl->seek(pos);
	}
	//
	// 关闭
	//
	void NodeReader::close() {
		mImpl->close();
	}

	//
	// 获取播放时长
	//
	int64_t NodeReader::getDurationUs() {
		return mImpl->getDurationUs();
	}
	//
	// 获取当前的播放位置
	//
	int64_t NodeReader::getCurrentUs() {
		return mImpl->getCurrentUs();
	}
}