#include "TimestampUtils.hpp"

namespace player {	
	//
	//  时间戳单位转到us值
	//
	int64_t timestampToUs(int64_t timestamp, const FFL::TimeBase& tb) {
		if (tb.mDen == 0) {
			FFL_ASSERT_LOG(0, "timestampToUs tb.den=0");
			return timestamp;
		}

		//av_rescale_q()
		//
		//  时间戳一个刻度对应多少毫秒
		//		
		double us = ((double)(1000 * 1000 * tb.mNum)) / tb.mDen;
		return (int64_t)(timestamp* us);
	}
	//
	//  时间戳单位转到us值
	//
	int64_t timestampToUs(int64_t timestamp, const FFL::TimeBase& tb, FFL::Clock* clock) {
		if (tb.mDen == 0) {
			FFL_ASSERT_LOG(0, "timestampToUs tb.den=0");
			return timestamp;
		}

		//av_rescale_q()
		//
		//  时间戳一个刻度对应多少毫秒
		//		
		double us = ((double)(1000 * 1000 * tb.mNum)) / tb.mDen;
		return (int64_t)(timestamp* us);
	}
	//
	//  us转时间戳单位
	//
	int64_t usToTimestamp(int64_t us, const FFL::TimeBase& tb) {
		if (tb.mDen == 0|| tb.mNum==0) {
			FFL_ASSERT_LOG(0, "usToTimestamp tb.den=0");
			return us;
		}

		//
		//  时间戳一个刻度对应多少毫秒
		//		
		double units = ((double)(1000 * 1000 * tb.mNum)) / tb.mDen;
		return (int64_t)(us/units);
	}
}
