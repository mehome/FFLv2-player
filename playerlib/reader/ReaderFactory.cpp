#include "ReaderFactory.hpp"
#include "FFmpegReader.hpp"

FFL_IMPLEMENT_SINGLETON(reader::ReaderFactory);
//template<> reader::ReaderFactory* FFL::Singleton<reader::ReaderFactory>::sInstance=NULL;
namespace reader {
    
	ReaderFactory::ReaderFactory():mReader(NULL){
	}
	ReaderFactory::~ReaderFactory() {
	}
	//
	//  根据url创建一个合适的ReaderInterface，如果不支持会返回NULL
	//
	FFL::sp<ReaderBase> ReaderFactory::createReader(const char* url, void* userdata){
		mReader = new FFMPegReader();
		return mReader;
	}
	//
	// 释放这个reader
	//
	void ReaderFactory::destroyReader(FFL::sp<ReaderBase> reader) {
		mReader = NULL;
	}
}
