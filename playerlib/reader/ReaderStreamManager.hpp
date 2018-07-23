#ifndef _STREAM_MANAGER_INTERFACE_HPP_
#define _STREAM_MANAGER_INTERFACE_HPP_
#include <ref/FFL_Ref.hpp>
#include "Stream.hpp"

namespace reader{
	class ReaderBase;
	class ReaderStreamManager {
	public:
		//
		//  添加reader下的一个stream到管理中，返回当前这个stream的id
		//
		virtual int32_t  addStream(ReaderBase* reader, StreamPtr stream) = 0;
		//
		//  添加reader下的所有stream到管理中，返回成功添加了几个流
		//  
		//
		virtual uint32_t addStreamVec(ReaderBase* reader, FFL::Vector < StreamPtr > streamVec) = 0;
		//
		//  根据流id获取一个流实例
		//
		virtual StreamPtr getStream(int32_t id) = 0;
		//
		//  获取这个reader下的所有流
		//
		virtual void getStreamVec(ReaderBase* reader, FFL::Vector < StreamPtr >& streamVec) = 0;
	};
}

#endif
