#ifndef _READER_FACTORY_HPP_
#define _READER_FACTORY_HPP_
#include "ReaderBase.hpp"
#include <utils/FFL_Singleton.hpp>
namespace reader {
	class ReaderFactory : public FFL::Singleton<ReaderFactory> {
		friend class FFL::Singleton<ReaderFactory>;
	private:
		ReaderFactory();
		~ReaderFactory();

	public:
		//
		//  根据url创建一个合适的ReaderInterface，如果不支持会返回NULL
		//
		FFL::sp<ReaderBase> createReader(const char* url,void* userdata);
		//
		// 释放这个reader
		//
		void destroyReader(FFL::sp<ReaderBase> reader);
	private:
		FFL::sp<ReaderBase> mReader;
	private:
		DISABLE_COPY_CONSTRUCTORS(ReaderFactory);
	};
}
#endif
