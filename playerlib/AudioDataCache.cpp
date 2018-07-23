
#include "AudioDataCache.hpp"
#include "PlayerConstant.hpp"
#include <utils/FFL_ByteStream.hpp>

namespace player {
	AudioDataCache::AudioDataCache():mByteBuffer(NULL), mStartup(false){
		mByteBuffer = new FFL::ByteBuffer();

	}
	AudioDataCache::~AudioDataCache() {
		FFL_SafeFree(mByteBuffer);
	}
	//
	//  设置大小
	//
	void AudioDataCache::setSize(uint32_t size) {
		mByteBuffer->alloc(size);
		mByteBuffer->getByteStream()->reset();
	}
	void AudioDataCache::startup() {
		mStartup = true;
	}
	void AudioDataCache::shutdown() {
		mStartup = false;
		mCond.signal();
	}
	//
	//  写一帧数据
	//
	bool AudioDataCache::writeFrame(AudioSample* samples) {		
		FFL::CMutex::Autolock l(mLock);
		FFL::ByteStream* stream = mByteBuffer->getByteStream();

		uint32_t offset = 0;
		uint32_t writeSize = (uint32_t)samples->mLinesize;
		while (mStartup) {
			uint32_t bufSize = stream->getCapacity() - stream->getSize();
			if (writeSize >= stream->getCapacity() && bufSize >= (stream->getCapacity() / 2)) {
				//
				//  拆开进行写
				//
				FFL_LOG_WARNING_TAG(TAG_AUDIO, "AudioDataCache::writeFrame sample size(%d)> cache size(%d).",
					writeSize, bufSize);
				stream->writeBytes((const int8_t*)samples->mData[0] + offset, bufSize);
				offset += bufSize;
			}
			else if (bufSize < writeSize) {
				//
				// 没有足够的缓冲区，等待一下
				//
				FFL_LOG_DEBUG_TAG(TAG_AUDIO, "AudioDataCache::writeFrame audio wait..writeSize=%d,bufSize=%d",
					writeSize, bufSize);
				mCond.wait(mLock);
			}
			else if (bufSize >= writeSize) {
				break;
			}
		}

		//
		//  写剩余部分数据
		//
		if (mStartup) {
			if ((uint32_t)samples->mLinesize > offset) {
				stream->writeBytes((const int8_t*)samples->mData[0] + offset, samples->mLinesize - offset);
			}

			SampleEntry entry;
			entry.pts = samples->mPts;
			entry.size = samples->mLinesize;
			entry.consumeCount = 0;			
			mSamples.push_back(entry);

			if (mCurrentRenderPts == 0) {
				mCurrentRenderPts = entry.pts;
			}
			return true;
		}

		return false;
	}
	//
	// 获取缓冲的数据量，还没有播放
	//
	int64_t AudioDataCache::getCacheBytes() {
		FFL::CMutex::Autolock l(&mLock);
		return mByteBuffer->getByteStream()->getSize();
	}
	//
	//  清空缓冲的数据
	//
	int64_t AudioDataCache::clearCache() {
		int64_t size = skip(0);
		mCurrentRenderPts = 0;
		return size;
	}
	//
	// 获取播放中的音频的pts
	//
	int64_t AudioDataCache::getRenderingPts() {
		FFL::CMutex::Autolock l(mLock);
		return mCurrentRenderPts;
	}	
	//
	//  从本地缓存读数据到交换缓冲中
	//
	uint32_t AudioDataCache::read(uint32_t wantedSize, FFL::ByteBuffer* buf) {
		FFL::CMutex::Autolock l(mLock);
		FFL::ByteStream* byteStream = mByteBuffer->getByteStream();
		if (byteStream->getSize() == 0) {
			mCond.signal();
			return 0;
		}

		uint32_t copyLen = 0;
		if (wantedSize > byteStream->getSize()) {
			copyLen = byteStream->getSize();
		}
		else {
			copyLen = wantedSize;
		}

		if (copyLen > 0) {
			if (buf->size() < copyLen) {
				buf->alloc(copyLen);
			}

			byteStream->readBytes((int8_t*)buf->data(), copyLen);
			if (byteStream->getSize() <= byteStream->getCapacity() / 2) {
				mCond.signal();
			}
		}
		else {
			FFL_LOG_WARNING_TAG(TAG_AUDIO, " AudioDataCache::readData2SwapBuffer wantedSize=%d read=0", wantedSize);
		}

		{
			int64_t count = copyLen;
			while (mSamples.size() > 0) {
				SampleEntry& entry = mSamples.front();
				entry.consumeCount += count;
				if (entry.size <= entry.consumeCount) {
					count = entry.consumeCount - entry.size;
					mCurrentRenderPts = entry.pts;
					mSamples.pop_front();
				}
				else {
					mCurrentRenderPts = entry.pts;
					break;
				}
			}
		}

		return copyLen;
	}

	//
	//  从本地缓存读数据到交换缓冲中
	//
	uint32_t AudioDataCache::skip(uint32_t wantedSize) {
		FFL::CMutex::Autolock l(mLock);
		FFL::ByteStream* byteStream = mByteBuffer->getByteStream();
		if (byteStream->getSize() == 0) {
			mCond.signal();
			return 0;
		}

		uint32_t copyLen = 0;
		if (wantedSize == 0) {
			copyLen = byteStream->getSize();
		}
		else if (wantedSize > byteStream->getSize()) {
			copyLen = byteStream->getSize();
		}
		else {
			copyLen = wantedSize;
		}

		if (copyLen > 0) {
			byteStream->readBytes(NULL, copyLen);
			if (byteStream->getSize() <= byteStream->getCapacity() / 2) {
				mCond.signal();
			}
		}
		else {
			FFL_LOG_WARNING_TAG(TAG_AUDIO, " AudioDataCache::skip wantedSize=%d read=0", wantedSize);
		}

		{
			int64_t count = copyLen;
			while (mSamples.size() > 0) {
				SampleEntry& entry = mSamples.front();
				entry.consumeCount += count;
				if (entry.size <= entry.consumeCount) {
					count = entry.consumeCount - entry.size;
					mCurrentRenderPts = entry.pts;
					mSamples.pop_front();
				}
				else {
					mCurrentRenderPts = entry.pts;
					break;
				}
			}
		}
		mCond.signal();
		return copyLen;
	}
}
