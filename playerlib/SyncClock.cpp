#include "SyncClock.hpp"
#include "TimestampUtils.hpp"

namespace player {
	SyncClock::SyncClock():mPause(false), mPauseClock(0){
		reset();
	}
	SyncClock::~SyncClock() {
	}
	//
	//  重置一下同步时钟
	//
	void SyncClock::reset() {
		mClock = 0;
		mWorldClock = 0;
	}
	//
	//  暂停时钟，
	//
	void SyncClock::pause() {
		mPause = true;
		mPauseClock = getClock();
	}
	void SyncClock::resume() {
		mPause = false;
		mPauseClock = 0;
	}
	//
	// 更新时钟
	//
	void SyncClock::updateClock(int64_t us) {
		mClock = us;
		mWorldClock = FFL_getNowUs();
	}
	void SyncClock::updateClock(int64_t timestamp, const FFL::TimeBase& tb) {
		updateClock(timestampToUs(timestamp, tb));
	}
	//
	// 获取当前时钟
	//
	int64_t SyncClock::getClock() {
		if (mClock == 0 || mWorldClock == 0) {
			return 0;
		}

		if (mPause) {
			return mPauseClock;
		}

		return mClock + FFL_getNowUs() - mWorldClock;
	}

	//
	//  计算2个时钟的差值 us
	//
	int64_t diffClock(SyncClock* c1, SyncClock* c2) {
		if (c1 == c2) {
			return 0;
		}
		return c1->getClock() - c2->getClock();
	}
}
