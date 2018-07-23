#ifndef _TIMESTAMPS_UTILS_HPP_
#define _TIMESTAMPS_UTILS_HPP_
#include <FFL.h>
#include <utils/FFL_Clock.hpp>

namespace player {	
	//
	//  时间戳单位转到us值
	//
	int64_t timestampToUs(int64_t timestamp,const FFL::TimeBase& tb);
	//
	//  时间戳单位转到us值
	//
	int64_t timestampToUs(int64_t timestamp, const FFL::TimeBase& tb,FFL::Clock* clock);
	//
	//  us转时间戳单位
	//
	int64_t usToTimestamp(int64_t us, const FFL::TimeBase& tb);
}

#endif
