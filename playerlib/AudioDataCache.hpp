
#ifndef _AUDIO_DATA_CACHE_HPP_
#define _AUDIO_DATA_CACHE_HPP_

#include <ref/FFL_Ref.hpp>
#include <utils/FFL_List.hpp>
#include <utils/FFL_Clock.hpp>
#include <thread/FFL_Mutex.hpp>
#include "AudioFormat.hpp"
#include "AudioSample.hpp"
#include <utils/FFL_ByteBuffer.hpp>
#include <utils/FFL_ByteStream.hpp>

namespace player {	
	class AudioDataCache {
	public:
		AudioDataCache();
		~AudioDataCache();

		//
		//  设置大小
		//
		void setSize(uint32_t size);

		void startup();
		void shutdown();

		//
		//  写一帧数据，可能会等待
		//
		bool writeFrame(AudioSample* samples);
		//
		// 获取缓冲的数据量，还没有播放
		//
		int64_t getCacheBytes();
		//
		//  清空缓冲的数据
		//
		int64_t clearCache();	
		//
		// 获取播放中的音频的pts
		//
		int64_t getRenderingPts();
		//
		//  从本地缓存读数据到交换缓冲中
		//
		uint32_t read(uint32_t wantedSize,FFL::ByteBuffer* buf);
		//
		//   wantedSize=0,跳过所有你的
		//
		uint32_t skip(uint32_t wantedSize);
	private:
		FFL::CMutex mLock;
		FFL::CCondition mCond;
		FFL::ByteBuffer* mByteBuffer;
		//
		// 用于sdl2和mByteBuffer间交换数据的,临时缓冲区
		//
		FFL::ByteBuffer mSwapBuffer;
	
		//
		//  保存写入的每一帧的信息
		//
		struct SampleEntry {
			int64_t pts;
			int64_t size;

			int64_t consumeCount;
		};
		FFL::List<SampleEntry> mSamples;		
		int64_t mCurrentRenderPts;

		volatile bool mStartup;
	};
}
#endif