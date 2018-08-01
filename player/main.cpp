/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  main.cpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/05/12 
 *  
 *  
 *
*/
#include <utils/FFL_MemoryWatch.hpp>
#include <logsender/FFL_LogSender.hpp>

extern int playerMain();
extern "C" int main(int argc ,const char* argv[]) {
	FFL::startMemoryWatch();
	FFL_LogSetLevel(FFL_LOG_LEVEL_ALL);

	FFL::LogSender sender;
	sender.startup();
	FFL_LOG_INFO("start LogSender");
	FFL_hookLogSystem(sender);
    
	FFL_LOG_INFO("start player");
	playerMain();
	FFL_LOG_INFO("quit player");
	sender.shutdown();

	FFL_dumpMemoryLeak();	
	return 0;
}
