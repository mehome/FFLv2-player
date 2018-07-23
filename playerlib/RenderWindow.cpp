/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Window.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/10
*
*  渲染窗口
*
*/

#include "RenderWindow.hpp"

namespace player {
	RenderWindow::RenderWindow():mHandle(NULL){
	}

	RenderWindow::~RenderWindow() {
	}

	void RenderWindow::setHandle(void* handle) {
		mHandle = handle;
	}

	void* RenderWindow::getHandle() {
		return mHandle;
	}
	//
	//  设置窗口的宽度，高度
	//
	void RenderWindow::setWindowSize(int32_t widht, int32_t height) {
	}
}