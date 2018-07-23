/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFMpeg.cpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/03/10 
 *  https://github.com/zhenfei2016/FFL-v2.git
 *
 *
 *
*/
#include "FFMpeg.hpp"
#include <FFL.h>




//
//  毫秒转化为ffmpeg的seekfile单位下的位置
//
int64_t FFMPegUsToSeekfileTimestamp(int64_t us){
	return us;
	//return av_rescale(us, AV_TIME_BASE, 1000000)
}