/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  player.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/06/20
*  https://github.com/zhenfei2016/FFL-v2.git
*  播放器测试程序
*
*/
#include <stdio.h>
#include <FFL_Commandline.h>
#include "SDL2Module.hpp"
#include "Player.hpp"

//
//  获取下一个视频url
//
static const char* getNextMovicUrl(bool next = true) {
	static const char* gMovicUrl[] = {
		"d://movic//sintel.ts",
		"d://movic//test.avi",
		"d://movic//zhuoyaoji.mp4",
	};

	static int gMovicCount = 3;
	static int gMovicIndex = -1;

	if (next) {
		gMovicIndex++;
	}
	if (gMovicIndex == -1) {
		gMovicIndex = 0;
	}
	int index = gMovicIndex %gMovicCount;
	return gMovicUrl[index];
}

//
// 开始播放
//
void play(const char* args, void* userdata) {
	printf("play: %s \n", args ? args : "null");
	player::FFLPlayer* player = (player::FFLPlayer*) userdata;
	player->setUrl(getNextMovicUrl());
	player->prepare();
}
//
// 播放下一条视频
//
void next(const char* args, void* userdata) {
	printf("play next: %s \n", args ? args : "null");
	player::FFLPlayer* player = (player::FFLPlayer*) userdata;
	player->setUrl(getNextMovicUrl(false));
	player->prepare();
}
//
// 暂停
//
void pause(const char* args, void* userdata) {
	printf("pause: %s \n", args ? args : "null");
	player::FFLPlayer* player = (player::FFLPlayer*) userdata;
	player->pause(1);
}
//
// 恢复继续播放
//
void resume(const char* args, void* userdata) {
	printf("resume: %s \n", args ? args : "null");
	player::FFLPlayer* player = (player::FFLPlayer*) userdata;
	player->pause(0);

}
//
// 停止播放
//
void stop(const char* args, void* userdata) {
	printf("stop: %s \n", args ? args : "null");
	player::FFLPlayer* player = (player::FFLPlayer*) userdata;
	player->stop();
}
//
// 退出exe
//
static bool gQuit = false;
void quit(const char* args, void* userdata) {
	printf("quit: %s \n", args ? args : "null");
	player::FFLPlayer* player = (player::FFLPlayer*) userdata;
	player->stop();
	gQuit = true;
}
//
//  向前跳2分钟
//
void jumpForward(const char* args, void* userdata) {
	player::FFLPlayer* player = (player::FFLPlayer*) userdata;


	int64_t cur = player->getCurrentPosition();
	int64_t target = cur + 2000 * 1000 * 60;
	if (target > player->getDuration()) {
		target = player->getDuration() - 10;
	}
	player->seekTo(target);

	char currentBuf[256] = {};
	FFL_usToString(cur, currentBuf);
	char targetBuf[256] = {};
	FFL_usToString(target, targetBuf);
	printf("jumpForward: cur=%s  target=%s \n", currentBuf, targetBuf);
}
//
//  向前后2分钟
//
void jumpBackword(const char* args, void* userdata) {
	player::FFLPlayer* player = (player::FFLPlayer*) userdata;


	int64_t cur = player->getCurrentPosition();
	int64_t target = cur - 2000 * 1000 * 60;
	if (target <= 0) {
		target = 0;
	}
	player->seekTo(target);

	char currentBuf[256] = {};
	FFL_usToString(cur, currentBuf);
	char targetBuf[256] = {};
	FFL_usToString(target, targetBuf);
	printf("jumpBackword: cur=%s  target=%s \n", currentBuf, targetBuf);
}

//
//  减小音量
//
void volumeDown(const char* args, void* userdata) {
	player::FFLPlayer* player = (player::FFLPlayer*) userdata;

	int32_t vol = player->getVolume();
	vol -= 5;
	vol = FFL_MAX(0, vol);
	player->setVolume(vol);

	printf("volumeDown: %d \n", vol);
}
//
// 增大音量
//
void volumeUp(const char* args, void* userdata) {
	player::FFLPlayer* player = (player::FFLPlayer*) userdata;

	int32_t vol = player->getVolume();
	vol += 5;
	vol = FFL_MIN(255, vol);
	player->setVolume(vol);

	printf("volumeUp: %d \n", vol);
}
//
// 快进
//
void fastForward(const char* args, void* userdata) {
	player::FFLPlayer* player = (player::FFLPlayer*) userdata;
	uint32_t speed = player->getSpeed();
	speed = FFL_MIN(speed + 5, 300);
	player->setSpeed(speed);
	printf("fastForward:%d  \n", speed);
}
//
// 快退
//
void fastReverse(const char* args, void* userdata) {
	player::FFLPlayer* player = (player::FFLPlayer*) userdata;
	uint32_t speed = player->getSpeed();
	speed = FFL_MAX(speed - 5, 10);
	player->setSpeed(speed);
	printf("fastReverse:%d  \n", speed);
}

//
// 设置loop的次数
//
void loopNum(const char* args, void* userdata) {
	player::FFLPlayer* player = (player::FFLPlayer*) userdata;
	int32_t num = player->getLoop();
	int32_t newNum=atoi(args);
	player->setLoop(newNum);
	printf("set loop num old=%d new=%d  \n", num, newNum);
}

class TestListener : public player::IPlayerListener {
public:
	TestListener(player::FFLPlayer* player) :mPlayer(player) {

	}

	virtual void onPrepared(int suc) {
		printf("onPrepared suc=%d\n", suc);
		if (suc) {
			mPlayer->start();
		}
	}
	//
	// 视频大小改变了
	// widht:宽度 ，height：高度
	// aspectRatioNum/aspectRatioDen  宽高比
	//
	virtual void onVideoSizeChanged(int32_t width, int32_t height, int32_t aspectRatioNum, int32_t aspectRatioDen) {
		printf("onVideoSizeChanged: width=%d height=%d aspectRatio=%d/%d\n",
			width, height,
			aspectRatioNum,
			aspectRatioDen);
		//mPlayer->setSurfaceSize(width,height);
	}
	//
	//  视频播放结束回调
	//
	virtual void onComplete() {
		printf("onComplete:  \n");
	}
	//
	//  seek结束回调
	//
	virtual void onSeekComplete(int64_t pos, int64_t duration) {
		char durationBuf[256] = {};
		FFL_usToString(duration, durationBuf);

		char posBuf[256] = {};
		FFL_usToString(pos, posBuf);
		printf("onSeekComplete: pos=%s duration=%s \n", posBuf, durationBuf);
	}
	//
	// 错误码
	//
	virtual void onError(int32_t errCode, int32_t errVal) {
		printf("onSeekComplete: errCode=%d errVal=%d \n", errCode, errVal);
	}
	//
	// 缓冲中
	//
	virtual void onBufferingStart() {
		printf("onBufferingStart:  \n");
	}
	virtual void onBufferingUpdate() {
		printf("onBufferingUpdate:  \n");
	}
	//
	// 一些其他的消息
	//
	virtual void onMessage(int32_t msg, int32_t parma1, int32_t param2) {
		printf("onMessage: msg=%d param1=%d parma2=%d\n", msg, parma1, param2);
	}

private:
	player::FFLPlayer* mPlayer;
};

class PlayerUiThread : public FFL::Thread {
public:
	PlayerUiThread(player::FFLPlayer *player) :mPlayer(player) {
	}
	virtual bool threadLoop() {
		mPlayer->create(NULL);
		player::SDL2Loop(NULL, mPlayer,&gQuit);
		return false;
	}

	player::FFLPlayer *mPlayer;
};


static void help(const char* args, void* userdata);
static CmdOption  gCmdOption[] = {
	{ "play",0,play,"play video " },
	{ "r",0,play,"play video " },
	{ "R",0,play,"play video " },

	{ "next",0,next,"play next video" },
	{ "n",0,next,"play next video" },
	{ "N",0,next,"play next video" },

	{ "pause",0,pause,"pause video" },
	{ "p",0,pause,"pause video" },
	{ "P",0,pause,"pause video" },
	
	{ "resume",0,resume,"resume video" },

	{ "stop",0,stop,"stop video" },
	{ "e",0,stop,"stop video" },
	{ "E",0,stop,"stop video" },

	{ "quit",0,quit,"quit exe" },
	{ "q",0,quit,"quit exe" },
	{ "Q",0,quit,"quit exe" },
	
	{ "jumpforward",0,jumpForward,"Jump one minute" },
	{ "d",0,jumpForward,"Jump one minute" },
	{ "D",0,jumpForward,"Jump one minute" },

	{ "jumpbackword",0,jumpBackword,"jumpBackword" },
	{ "a",0,jumpForward,"jumpForward" },
	{ "A",0,jumpForward,"jumpForward" },
	
	{ "volumedown",0,volumeDown,"turn down the volume" },
	{ "s",0,volumeDown,"turn down the volume" },
	{ "S",0,volumeDown,"turn down the volume" }, 

	{ "volumeup",0,volumeUp,"turn up the volume" },
	{ "w",0,volumeUp,"turn up the volume" },
	{ "W",0,volumeUp,"turn up the volume" },

	{ "fastforward",0,fastForward,"Increase the playback speed" },
	{ "]",0,fastForward,"Increase the playback speed" },
	{ "}",0,fastForward,"Increase the playback speed" },

	{ "fastreverse",0,fastReverse,"Slow down the playback speed" },
	{ "{",0,fastReverse,"Slow down the playback speed" },
	{ "[",0,fastReverse,"Slow down the playback speed" },

	{ "loop",0,loopNum,"Set loop num. " },
	{ "help",0,help,"prinf help" },

	{ 0,0,0,0 }
};

static void help(const char* args, void* userdata) {

	printf("--------------------usage-----------------------------------------\n");
	for (int32_t i = 0; i < FFL_ARRAY_ELEMS(gCmdOption); i++) {
		if (gCmdOption[i].mName) {
			printf("%s  :%s \n", gCmdOption[i].mName, gCmdOption[i].nHelp);
		}
	}
	printf("-----------------------------------------------------------------\n");
}

int playerMain() {
	help(0,0);
	
	player::FFLPlayer player;
	TestListener listener(&player);
	player.setListener(&listener);
	FFL::sp<PlayerUiThread>  uiThread= new PlayerUiThread(&player);
	uiThread->run("ui");

	char cmd[256] = {};
	cmd[0] = '-';
	cmd[1] = '-';
	const char *argv[] = {
		"",
		cmd,
	};
	int argc = 2;

	while (fgets(cmd + 2, 256 - 3, stdin)) {		
		//
		//  把输入命令格式化为  cmd=xxx  ,就是把命令转化成第一个参数
		//		
		char* pCmdLine = cmd + 2;
		while (*pCmdLine++) {
			if (*pCmdLine == ' ' || *pCmdLine == '\n' || *pCmdLine == '\r') {
				*pCmdLine = '=';
				break;
			}
		}

		if (FFL_parseCommnadline(argc, argv,
			gCmdOption,
			FFL_ARRAY_ELEMS(gCmdOption),
			&player) < 0) {
			printf("unknown command: %s \n", cmd + 2);
		}

		if (gQuit) {
			break;
		}
	}

	uiThread->requestExitAndWait();

	return 0;
}
