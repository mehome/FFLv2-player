#include "ReaderInterface.hpp"
#include "StreamManagerInterface.hpp"
namespace reader {



	ReaderInterface::ReaderInterface():mStreamMgr(&gDefaultStreamMgr){
	}
	ReaderInterface::~ReaderInterface(){
	}
	//
	//  设置流管理器,当open成功后，读取到所有流基本信息，然后会通知IStreamManager
	//
	void ReaderInterface::setStreamManager(StreamMgrInterface* streamMgr) {
		if (streamMgr) {
			mStreamMgr = streamMgr;
		}
	}
	//
	//  获取流管理
	//
	StreamMgrInterface* ReaderInterface::getStreamManager() {
		return mStreamMgr;
	}
}