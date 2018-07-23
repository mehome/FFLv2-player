#include <SDL.h>
#include <utils/FFL_ByteStream.hpp>
#include "SDL2AudioDevice.hpp"
#include "PlayerConstant.hpp"
#include "AudioUtils.hpp"

namespace player {
	//
	//  AudioFormat格式到sdl2格式映射表
	//
	struct AudioFormatToSDL2Map {
		AVSampleFormat mAudioFormat;
		int32_t mSDL2Format;
	};
	static AudioFormatToSDL2Map gSupportMap[] = {		
		{ AV_SAMPLE_FMT_S16,AUDIO_S16SYS },
		{ AV_SAMPLE_FMT_S32,AUDIO_S32SYS },
		{ AV_SAMPLE_FMT_FLT,AUDIO_F32SYS },		
	};
	static int32_t ffmpegFormat2SDL2Format(AVSampleFormat fmt) {
		for (uint32_t i = 0; i < FFL_ARRAY_ELEMS(gSupportMap); i++) {			
			if (fmt == gSupportMap[i].mAudioFormat) {
				return gSupportMap[i].mSDL2Format;
			}			
		}
		return -1;
	}	
	static AVSampleFormat SDL2Format2ffmpegFormat(int32_t fmt) {
		for (uint32_t i = 0; i < FFL_ARRAY_ELEMS(gSupportMap); i++) {
			if (fmt == gSupportMap[i].mSDL2Format) {
				return gSupportMap[i].mAudioFormat;
			}
		}
		return AV_SAMPLE_FMT_NONE;
	}

	SDL2AudioDevice::SDL2AudioDevice():mCacheUs(1000*500){			
		mIsOpened = false;
		mBytesPerSec = 0;
	}
	SDL2AudioDevice::~SDL2AudioDevice() {
		close();		
	}
	//
	//  获取支持的格式
	//  wanted: 如果为nUll则返回所有支持的格式
	//           非null 返回跟他匹配的 
	//  fmtList: 返回支持的格式list
	//
	void SDL2AudioDevice::getSupportFormat(const AudioFormat* wanted, FFL::List<AudioFormat>& fmtList) {
		for (uint32_t i = 0; i < FFL_ARRAY_ELEMS(gSupportMap); i++) {			
			AudioFormat fmt;
			fmt.mFormat = gSupportMap[i].mAudioFormat;
			fmtList.push_back(fmt);
		}
	}
	bool SDL2AudioDevice::isSupportFormat(const AudioFormat* wanted) {
		if (wanted) {
			for (uint32_t i = 0; i < FFL_ARRAY_ELEMS(gSupportMap); i++) {
				if (gSupportMap[i].mAudioFormat == wanted->mFormat)
					return true;		
			}
		}
		return false;
	}

	//
	//  获取相似的音频格式
	//
	bool SDL2AudioDevice::getSimilarFormat(const AudioFormat* wanted,AudioFormat& obtained) {
		if (isSupportFormat(wanted)) {
			obtained = *wanted;
			return  true;
		}

		for (uint32_t i = 0; i < FFL_ARRAY_ELEMS(gSupportMap); i++) {
			if (gSupportMap[i].mAudioFormat == wanted->mFormat)
				return true;
		}

		obtained = *wanted;
		obtained.mFormat = gSupportMap[0].mAudioFormat;
		return true;
	}
	//
	// 设置缓冲多长时间的数据
	//
	void SDL2AudioDevice::setCacheUs(int64_t us) {
		mCacheUs = us;
	}
	//
	//  打开关闭音频设备
	//
	bool SDL2AudioDevice::onOpen(const AudioFormat& wanted, int32_t sampleNum, AudioFormat& obtained) {
		AudioFormat inFormat;
		if (isSupportFormat(&wanted)) {
			inFormat = wanted;
		}
		else {
			getSimilarFormat(&wanted, inFormat);
		}

		SDL_AudioSpec wantedSpec = {};
		wantedSpec.freq = inFormat.mFreq;
		wantedSpec.format = ffmpegFormat2SDL2Format(inFormat.mFormat);
		wantedSpec.channels = inFormat.mChannelNum;
		wantedSpec.silence = 0;
		wantedSpec.samples = FFL_MAX(sampleNum, 1024) ;
		wantedSpec.callback = SDL2AudioDevice::SDL2_fillAudio;
		wantedSpec.userdata = this;

		SDL_AudioSpec  obtainedSpec = {};
		if (SDL_OpenAudio(&wantedSpec, &obtainedSpec) < 0) {
			FFL_LOG_ERROR_TAG(TAG_AUDIO,"Faild to SDL2AudioDevice::open");
			return false;
		}
		mDeviceDelayUs = (int64_t (obtainedSpec.samples * 1000 *1000)) / obtainedSpec.freq;
		obtained.mFreq = obtainedSpec.freq;
		obtained.mFormat = SDL2Format2ffmpegFormat(obtainedSpec.format);
		obtained.mChannelNum = obtainedSpec.channels;
		mObtainedFormat = obtained;

		//
		// 计算每秒消耗多少字节,申请内存用于缓冲音频数据
		//
		uint32_t perSampleSize = obtained.getPerFrameSize();
		mBytesPerSec = obtained.mFreq * perSampleSize;

		uint32_t bufSize =(uint32_t) (mBytesPerSec * ((double)mCacheUs / (1000 * 1000)));
		bufSize = FFL_MAX(sampleNum * perSampleSize* 4, bufSize);
		mDataCache.setSize(bufSize);
		mDataCache.startup();
		
		SDL_PauseAudio(0);
		mIsOpened = true;
		return mIsOpened;;
	}
	void SDL2AudioDevice::onClose() {
		if (mIsOpened) {
			SDL_PauseAudio(1);
			SDL_CloseAudio();
			mIsOpened = false;
			mDataCache.shutdown();
		}
	}
	//
	//获取当前打开的格式，如果未打开则返回null
	//
	AudioFormat* SDL2AudioDevice::getOpenFormat() {
		if (mIsOpened) {
			return &mObtainedFormat;
		}
		return NULL;
	}
	//
	//  写一帧数据
	//
	bool SDL2AudioDevice::writeFrame(AudioSample* samples) {
		if (!isOpened()) {			
			FFL_LOG_WARNING_TAG(TAG_AUDIO, "Failed to SDL2AudioDevice::writeFrame device not open.");
			return false;
		}
		return mDataCache.writeFrame(samples);
	}
	//
	//  获取播放设备硬件的延迟
	//
	int64_t SDL2AudioDevice::getDeviceDelayUs() {
		return mDeviceDelayUs;
		//return 0;
	}
	//
	// 获取缓冲的数据量，还没有播放
	//
	int64_t SDL2AudioDevice::getCacheBytes() {		
		return mDataCache.getCacheBytes();
	}
	//
	//  清空缓冲的数据
	//
	int64_t SDL2AudioDevice::clearCache() {						
		return mDataCache.clearCache();
	}
	//
	// 获取缓冲的延迟时间
	//
	int64_t SDL2AudioDevice::getCacheDelayUs() {		
		int64_t bytes = getCacheBytes();
		return (int64_t)((double)(bytes * 1000 * 1000) / mBytesPerSec);
	}
	int64_t SDL2AudioDevice::getCacheDelay(FFL::TimeBase& base) {	
		int64_t bytes = getCacheBytes();		
		return (int64_t)((double)(bytes *  base.mDen) / (mBytesPerSec * base.mNum));
	}
	//
	// 获取播放中的音频的pts
	//
	int64_t SDL2AudioDevice::getRenderingPts() {
		return mDataCache.getRenderingPts();
	}
	//
	//  sdl2回调获取音频数据
	//
	void  SDL2AudioDevice::SDL2_fillAudio(void *userdata, Uint8 *stream, int len) {
		SDL2AudioDevice* pThis = (SDL2AudioDevice*)userdata;
		if (stream && len) {
			pThis->SDL2_fill(stream, len);
		}
	}	
	void SDL2AudioDevice::SDL2_fill(uint8_t *stream, uint32_t len) {
		SDL_memset(stream, 0, len);
		uint32_t readed=mDataCache.read(len,&mSwapBuffer);		
		if (readed != len) {
			FFL_LOG_DEBUG_TAG(TAG_AUDIO, "SDL2AudioDevice::SDL2_fill readed=%d  want=%d", readed , len);
		}

		int32_t volume=getVolume();	

		int sdlVolume = (int)(((float)volume / 255) * SDL_MIX_MAXVOLUME);
		SDL_MixAudio(stream, (Uint8*)mSwapBuffer.data(), readed, sdlVolume);
	}		
}
