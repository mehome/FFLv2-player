#ifndef _READER_BASE_HPP_
#define _READER_BASE_HPP_
#include "../NodeBase.hpp"
#include <pipeline/FFL_PipelineEventCallback.hpp>

namespace reader {
	class ReaderStreamManager;
	class ReaderBase : public player::NodeBase {
	public:
		ReaderBase();
		~ReaderBase();
	protected:
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		//    player::NodeBase
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//  成功创建了node
		//
		virtual void onCreate();
		//
		//  成功删除了node
		//
		virtual void onDestroy();
	public:
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		//    ReaderInterface
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//  打开
		//
		virtual void open(const char* url) ;
		//
		//  暂停读，但是不关闭
		//
		virtual void pause() ;
		//
		//  恢复读
		//
		virtual void resume() ;
		//
		// 设置开始读位置
		//
		virtual void seek(int64_t pos) ;
		//
		// 关闭
		//
		virtual void close() ;
		//
		// 获取播放时长us
		//
		virtual int64_t getDuration() = 0;
		//
		// 获取当前的播放位置 us
		//
		virtual int64_t getCurrentPosition() = 0;

		//
		// 获取，设置循环播放次数
		// 如果<0 : 一直循环播放
		//     =0 : 播放一次
		//     >0 : 播放num+1次
		//
		virtual void setLoopNum(int32_t count);
		virtual int32_t getLoopNum();
	public:
		//
		//  设置流管理器,当open成功后，读取到所有流基本信息，然后会通知IStreamManager
		//
		void setStreamManager(ReaderStreamManager* streamMgr) {
			if (streamMgr) {
				mStreamMgr = streamMgr;
			}
		}
		//
		//  获取流管理,不会返回null的
		//
		ReaderStreamManager* getStreamManager() const{
			return mStreamMgr;
		}
	private:
		ReaderStreamManager* mStreamMgr;
	public:
		//
		// 是否暂停状态
		//
		bool isPaused() const;
	protected:
		player::InputInterface mInput;
		bool initLooper();
		void pauseLooper();
	protected:
		friend class FFL::ClassMethodVoidInputHandler<ReaderBase>;
		//
		// 读取主循环
		//
		void onReadOnce(const FFL::sp<FFL::PipelineMessage>& msg, void* arg);
		//
		//  读取一帧数据
		//
		virtual void onReadFrame(int64_t trackId)=0;
	protected:
		friend class FFL::ClassMethodCallback<ReaderBase>;
		FFL::CMutex mRequestLock;
		//
		// open函数，具体实现
		//
		volatile bool mEventOpenPending;
		FFL::String mUrl;
		FFL::sp<FFL::PipelineEvent> mEventOpen;
		void onOpenStub(const FFL::sp<FFL::PipelineEvent>& event);
		virtual void onOpen(const char* url)=0;

		//
		// close函数，具体实现
		//
		volatile bool mEventClosePending;
		FFL::sp<FFL::PipelineEvent> mEventClose;
		void onCloseStub(const FFL::sp<FFL::PipelineEvent>& event);
		virtual void onClose()=0;

		//
		// pause函数，resume 具体实现
		//
		volatile bool mEventPausePending;
		volatile bool mEventResumePending;
		volatile bool mIsPaused;
		FFL::CMutex mPauseLock;
		FFL::CCondition mPauseCond;
		FFL::sp<FFL::PipelineEvent> mEventPause;
		void onPauseStub(const FFL::sp<FFL::PipelineEvent>& event);
		virtual void onPause()=0;
		virtual void onResume()=0;

		//
		// seek函数，具体实现
		//
		volatile bool mEventSeekPending;		
		FFL::sp<FFL::PipelineMessage> mMsgSeek;
		void onSeekStub(const FFL::sp<FFL::PipelineMessage>& event);
		virtual bool onSeek(int64_t pos)=0;		

	private:
		FFL::sp<FFL::PipelineSourceConnector> mSourceConn;
	protected:
		//
		// 循环播放次数
		//
		volatile int32_t mLoopCount;
	};
}
#endif
