#include "ReaderBase.hpp"
#include "../PlayerCore.hpp"

namespace reader {
	class DefaultStreamMgr : public ReaderStreamManager {
	public:
		//
		//  添加reader下的一个stream到管理中，返回当前这个stream的id
		//
		virtual int32_t  addStream(ReaderBase* reader, StreamPtr stream) {
			return -1;
		}
		//
		//  添加reader下的所有stream到管理中，返回成功添加了几个流
		//  
		//
		virtual uint32_t addStreamVec(ReaderBase* reader, FFL::Vector < StreamPtr > streamVec) {
			return 0;
		}
		//
		//  根据流id获取一个流实例
		//
		virtual StreamPtr getStream(int32_t id) {
			return NULL;
		}
		//
		//  获取这个reader下的所有流
		//
		virtual void getStreamVec(ReaderBase* reader, FFL::Vector < StreamPtr >& streamVec) {
			streamVec.clear();
		}
	};
	static DefaultStreamMgr gDefaultStreamMgr;

	ReaderBase::ReaderBase():
		mStreamMgr(&gDefaultStreamMgr),
		mEventOpenPending(false),
		mEventClosePending(false),
		mEventPausePending(false),
		mEventResumePending(false),
		mIsPaused(false),
		mEventSeekPending(false){
		mUrl = "";

		mLoopCount = 0;
		mEventOpen = new FFL::PipelineEvent(
			new  FFL::ClassMethodCallback<ReaderBase>(this,&ReaderBase::onOpenStub));
		mEventClose = new FFL::PipelineEvent(
			new  FFL::ClassMethodCallback<ReaderBase>(this, &ReaderBase::onCloseStub));
		mEventPause = new FFL::PipelineEvent(
			new  FFL::ClassMethodCallback<ReaderBase>(this, &ReaderBase::onPauseStub));
		mMsgSeek = new FFL::PipelineMessage();

	}
	ReaderBase::~ReaderBase() {

	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//    player::NodeBase
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//  成功创建了node
	//
	void ReaderBase::onCreate() {
		initLooper();
	}
	//
	//  成功删除了node
	//
	void ReaderBase::onDestroy() {

	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//    ReaderInterface
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//  打开
	//
	void ReaderBase::open(const char* url){		
		FFL::CMutex::Autolock l(&mRequestLock);
		if (mEventOpenPending) {
			return;
		}

		mUrl = url;
		mEventOpenPending = true;
		postEvent(mEventOpen);
	}
	//
	//  暂停读，但是不关闭
	//
	void ReaderBase::pause(){
		FFL::CMutex::Autolock l(&mRequestLock);
		if (mEventPausePending && mIsPaused) {
			return;
		}
		mEventPausePending = true;
		mEventPause->setParams(0, 0);
		postEvent(mEventPause);
	}
	//
	//  恢复读
	//
	void ReaderBase::resume(){
		FFL::CMutex::Autolock l(&mRequestLock);
		if (mEventResumePending || !mIsPaused) {
			return;
		}
		mEventResumePending = true;		
		mEventPause->setParams(1, 0);
		postEvent(mEventPause);
		mPauseCond.signal();
	}
	//
	// 设置开始读位置
	//
	void ReaderBase::seek(int64_t pos){
		FFL::CMutex::Autolock l(&mRequestLock);
		if (mEventSeekPending) {
			return;
		}

		mEventSeekPending = true;
		mMsgSeek->setParams(pos, 0);
		mSourceConn->tranport(mMsgSeek, 0);
		mPauseCond.signal();
	}
	//
	// 关闭
	//
	void ReaderBase::close(){
		FFL::CMutex::Autolock l(&mRequestLock);
		if (mEventClosePending) {
			return;
		}
		mEventClosePending = true;
		mPauseCond.signal();
	}
	//
	// 获取，设置循环播放次数
	// 如果<0 : 一直循环播放
	//     =0 : 播放一次
	//     >0 : 播放num+1次
	//
	void ReaderBase::setLoopNum(int32_t count) {
		mLoopCount = count;
	}
	int32_t ReaderBase::getLoopNum() {
		return mLoopCount;
	}
	//
	// 是否暂停状态
	//
	bool ReaderBase::isPaused() const {
		return mIsPaused;
	}
	bool  ReaderBase::initLooper(){
		FFL::sp<FFL::PipelineInputHandler> callback =
			new FFL::ClassMethodInputHandler<ReaderBase,void*>(this, &ReaderBase::onReadOnce,0);
		player::InputInterface input = createInputInterface(callback, "reader-stream");
		if (!input.isValid()) {
			FFL_LOG_ERROR("invalid input id");
			return false;
		}

		mInput = input;
		FFL::sp<FFL::PipelineSourceConnector> conn = 0;
		if (1) {
			conn = new FFL::PipelineIdleSourceConnector();
		}
		else {
			conn = new FFL::PipelineTimerSourceConnector(3000);
		}		
		
		mSourceConn = conn;
		getOwner()->getPipeline()->connectSource(input.mNodeId, input.mId, conn);
		return true;
	}
	void ReaderBase::pauseLooper() {
		//
		//  关闭当前的输入
		//
		FFL::sp<FFL::PipelineInput> input = getInput(mInput.mId);
		if (!input.isEmpty()) {
			input->requestShutdown();
		}
	}
	//
	// 读取主循环
	//
	void ReaderBase::onReadOnce(const FFL::sp<FFL::PipelineMessage>& msg, void* arg){
		if (!msg.isEmpty()) {
			if (mMsgSeek.get() == msg.get()) {
				onSeekStub(mMsgSeek);
				return;
			}
		}

		if (mIsPaused) {
			mPauseCond.waitRelative(mPauseLock, 100);
			return;
		}		
		onReadFrame(msg->trackId());
		msg->consume(this);
	}
	void ReaderBase::onOpenStub(const FFL::sp<FFL::PipelineEvent>& event) {
		onOpen(mUrl.c_str());
		{
			FFL::CMutex::Autolock l(&mRequestLock);
			mEventOpenPending = false;
		}
	}	
	//
	// close函数，具体实现
	//
	void ReaderBase::onCloseStub(const FFL::sp<FFL::PipelineEvent>& event) {
		onClose();
		{
			FFL::CMutex::Autolock l(&mRequestLock);
			mEventClosePending = false;
		}
	}


	//
	// pause函数，resume 具体实现
	//
	void ReaderBase::onPauseStub(const FFL::sp<FFL::PipelineEvent>& event) {
		if (event->getParam1() == 0) {
			onPause();
			{
				FFL::CMutex::Autolock l(&mRequestLock);
				mIsPaused = true;
				mEventPausePending = false;				
			}
			event::postPlayerEvent(getOwner(), event::EVENT_PAUSE, 0,0);
		}
		else {
			onResume();
			{
				FFL::CMutex::Autolock l(&mRequestLock);
				mIsPaused = false;
				mEventResumePending = false;
			}

			event::postPlayerEvent(getOwner(), event::EVENT_RESUME, 0,0);
		}
	}
	//
	// seek函数，具体实现
	//
	void ReaderBase::onSeekStub(const FFL::sp<FFL::PipelineMessage>& event) {
		onSeek(event->getParam1());
		{
			FFL::CMutex::Autolock l(&mRequestLock);
			mEventSeekPending = false;
		}
	}
}
