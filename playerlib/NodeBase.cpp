/* 
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  NodeBase.cpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/03/11 
 *  
 *  播放器中节点的基类
 *
*/
#include "NodeBase.hpp"
#include "PlayerCore.hpp"
#include <pipeline/FFL_PipelineSourceConnector.hpp>
#include "MessageFFMpegPacket.hpp"

namespace player {
	class NodeBase::NodeHandler : public FFL::PipelineNodeHandler {
	public:
		NodeHandler(NodeBase* node) :mNodeBase(node) {
		}
		~NodeHandler() {
		}
		//
		//  返回false则node不进行启动了
		//
		bool onStartupBefore() {
			FFL_LOG_INFO("[%s] onStartupBefore %p", mNodeBase->mNodeName.c_str(), this);
			return true;
		}
		void onStartupAfter() {
			FFL_LOG_INFO("[%s] onStartupAfter %p", mNodeBase->mNodeName.c_str(), this);
		}

		void onShutdownBefore() {
			FFL_LOG_INFO("[%s] onShutdownBefore %p", mNodeBase->mNodeName.c_str(), this);
		}
		void onShutdownAfter() {
			FFL_LOG_INFO("[%s] onShutdownAfter %p", mNodeBase->mNodeName.c_str(), this);
		}

		void onStartMessageLooper(FFL::PipelineInputId id) {
			FFL_LOG_INFO("[%s:%d] onStartMessageLooper %p", mNodeBase->mNodeName.c_str(), id, this);
			mNodeBase->onStartMessageLooper();
		}
		bool prepareHandleMessage(FFL::PipelineInputId id, const FFL::sp<FFL::PipelineMessage> &msg)
		{
			FFL_LOG_INFO("[%s:%d] prepareHandleMessage %p", mNodeBase->mNodeName.c_str(), id, this);
			return true;
		}
		//
		//  退出looper的消息
		//
		void onQuitMessageLooper(FFL::PipelineInputId id, uint32_t exitCode) {
			FFL_LOG_INFO("[%s:%d] onQuitMessageLooper %p", mNodeBase->mNodeName.c_str(), id, this);
			mNodeBase->onQuitMessageLooper();
		}

		void onEvent(const FFL::sp<FFL::PipelineEvent> &msg) {
			FFL_LOG_INFO("[%s] onEvent %p", mNodeBase->mNodeName.c_str(), this);
		}
	private:
		NodeBase* mNodeBase;
	};

	class PlayerCore;
	NodeBase::NodeBase() : mNodeId(FFL::Pipeline_INVALID_Id)
	{
		mPlayer = NULL;
		mNodeHandler = new NodeHandler(this);		
	}

	NodeBase::~NodeBase()
	{}
	//
	// 开始创建这个节点
	//
	status_t NodeBase::create(PlayerCore* player) {
		if (mPlayer != NULL || player==NULL) {
			FFL_LOG_WARNING("NodeBase::create fail ");
			return FFL_ERROR_FAIL;
		}

		mPlayer = player;	
		
		FFL::sp<FFL::Pipeline> pipeline = mPlayer->getPipeline();		
		mNodeId = pipeline->createNode(mNodeHandler);
		if (mNodeId == FFL::Pipeline_INVALID_Id) {
			FFL_LOG_ERROR("invalid node id");
		}
		FFL_LOG_INFO("create \"%s\" nodeid=%d",mNodeName.c_str(),getNodeId());
		mPlayer->registerNode(this);
		onCreate();
		return FFL_OK;
	}
	void NodeBase::destroy() {
		if (isCreated()) {
			mPlayer->unRegisterNode(this);
			mPlayer = NULL;
			onDestroy();
		}
	}	
	bool NodeBase::isCreated() const{
		return mPlayer != NULL;
	}
	void NodeBase::setName(const char* name)
	{
		mNodeName = name ? name : "";
	}
	void NodeBase::getName(FFL::String& name) const {
		name = mNodeName;
	}
	const char* NodeBase::getName()  {
		return mNodeName.c_str();
	}
	//
	//  获取这个node的所有者player
	//
	PlayerCore* NodeBase::getOwner() const {
		return mPlayer;
	}
	//
	//  获取nodeid
	//
	FFL::PipelineNodeId NodeBase::getNodeId() const {
		return mNodeId;
	}
	//
	//  创建输入接口
	//
	InputInterface NodeBase::createInputInterface(FFL::sp<FFL::PipelineInputHandler> callback, const char* name) {
		InputInterface input(getNodeId(), FFL::Pipeline_INVALID_Id);
		if (isCreated()) {
			FFL::sp<FFL::Pipeline> pipeline = mPlayer->getPipeline();
			input.mId = pipeline->createInput(getNodeId(), callback, name);
			if (!input.isValid()) {
				FFL_LOG_ERROR("invalid input id");
			}
		}
		else {
			FFL_LOG_WARNING("failed to NodeBase::createInputInterface. Node not create. ");
		}
		return input;
	}
	//
	//   创建输出接口
	//
	OutputInterface NodeBase::createOutputInterface() {
		OutputInterface output(getNodeId(), FFL::Pipeline_INVALID_Id);
		if (isCreated()) {
			FFL::sp<FFL::Pipeline> pipeline = mPlayer->getPipeline();		
			output.mId = pipeline->createOutput(getNodeId());
			if (!output.isValid()) {
				FFL_LOG_ERROR("invalid input id");
			}
		}
		else {
			FFL_LOG_WARNING("failed to NodeBase::createOutputInterface. Node not create. ");
		}
		return output;
	}

    //
    //  获取输入
    //
    FFL::sp<FFL::PipelineInput> NodeBase::getInput(FFL::PipelineInputId id){
        return mNodeHandler->getInput(id);
    }
	FFL::sp<FFL::PipelineOutput> NodeBase::getOutput(FFL::PipelineOutputId id) {
		return mNodeHandler->getOutput(id);
	}
	//
	//  连接一个输出到一个输入
	//
	bool NodeBase::connect(const OutputInterface& output, const InputInterface& input,
		FFL::sp<FFL::PipelineConnector> conn) {
		if (conn.isEmpty()) {
			return false;
		}

		if (getOwner()->getPipeline()->connect(output.mNodeId, output.mId,
			input.mNodeId, input.mId, conn) == FFL_OK) {
			return true;
		}

		return false;
	}

	//
	//  output:数据提供者输出接口
	//	input：返回当前的这个节点的输入接口接口
	//  返回是否成功
	//
	bool NodeBase::connectSource(const OutputInterface& output,const char* name,InputInterface& input,void* userdata) {
		//
		FFL::sp<FFL::PipelineInputHandler> handler =
			new FFL::ClassMethodInputHandler<NodeBase,void* >(this, &NodeBase::receiveData,userdata);
		input = createInputInterface(handler, name);

		FFL::sp<FFL::PipelineConnector > conn = onCreateConnector(output, input,userdata);
		if (connect(output, input, conn)) {
			return true;
		}else {
			FFL_LOG_WARNING("(%s)NodeBase::connectSource failed", mNodeName.c_str());
		}

		return false;
	}

	//
	// 收到消息 ,connectSource建立的连接，连接上的数据来了会触发这个函数
	//
	void NodeBase::receiveData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
        handleReceivedData(msg, userdata);
	}

	//
	//   外部setDataInput时候调用此函数，创建对应conn
	//
	FFL::sp<FFL::PipelineConnector > NodeBase::onCreateConnector(
		const OutputInterface& output,
		const InputInterface& input,
		void* userdata) {
		return new FFL::SyncPipelineConnector();
	}
	FFL::sp<FFL::PipelineConnector> NodeBase::getConnector(const OutputInterface& output) {
		FFL_ASSERT(0);
		return NULL;
	}
	FFL::sp<FFL::PipelineConnector> NodeBase::getConnector(const InputInterface& input) {
		FFL_ASSERT(0);
		return NULL;
	}
	//
	// 处理接收到的消息，
	//
	bool NodeBase::handleReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
		return false;
	}
	//
	//  发送事件消息
	//
	void NodeBase::postEvent(const FFL::sp<FFL::PipelineEvent> &msg) {
		mNodeHandler->postEvent(msg);
	}
	//
	//  发送消息到output		
	//  dstId: 目标id
	//  msg : 消息内容
	//
	status_t NodeBase::postMessage(FFL::PipelineOutputId dstId, const FFL::sp<FFL::PipelineMessage> &msg) {
		return mNodeHandler->postMessage(dstId, msg, 0);
	}
	status_t NodeBase::postMessageDelay(FFL::PipelineOutputId dstId, const FFL::sp<FFL::PipelineMessage> &msg, uint64_t delayUs = 0) {
		return mNodeHandler->postMessage(dstId, msg, delayUs);
	}
	//
	//  给当前节点的一个输入接口发送一条消息
	//
	status_t NodeBase::postSelfMessage(FFL::PipelineInputId dstId, const FFL::sp<FFL::PipelineMessage> &msg, uint64_t delayUs) {
		return mNodeHandler->postSelfMessage(dstId, msg, delayUs);
	}
	//
	//   清空输出节点中没有处理的消息
	//
	void NodeBase::clearMessage(FFL::PipelineInputId dstId) {
		FFL::sp<FFL::PipelineOutput > output = getOutput(dstId);
		if (!output.isEmpty()) {
			output->clearMessage();
		}else {
			FFL_LOG_WARNING("NodeBase(%s)::clearMessage not find dstid=%d ", dstId);
		}
	}
	void NodeBase::onCreate() {}
	void NodeBase::onDestroy(){}
	void NodeBase::onStartMessageLooper()
	{}
	void NodeBase::onQuitMessageLooper()
	{}
	//
	//  获取这个节点属于哪一个线的
	//
	FFL::sp<FFL::Pipeline> NodeBase::getPipeline()
	{		
		return mPlayer->getPipeline();	
	}


	InputInterface::InputInterface() :
		mNodeId(FFL::Pipeline_INVALID_Id),
		mId(FFL::Pipeline_INVALID_Id) {
	}
	InputInterface::InputInterface(FFL::PipelineNodeId nodeId, FFL::PipelineInputId inputId) :
		mNodeId(nodeId), mId(inputId)
	{
	}
	bool InputInterface::isValid() const {
		return mNodeId != FFL::Pipeline_INVALID_Id && mId != FFL::Pipeline_INVALID_Id;
	}

	OutputInterface::OutputInterface() :
		mNodeId(FFL::Pipeline_INVALID_Id),
		mId(FFL::Pipeline_INVALID_Id) {
	}
	OutputInterface::OutputInterface(FFL::PipelineNodeId nodeId, FFL::PipelineOutputId outputId) :
		mNodeId(nodeId), mId(outputId)
	{
	}
	void OutputInterface::reset(){
		mNodeId=FFL::Pipeline_INVALID_Id;
		mId=FFL::Pipeline_INVALID_Id;
	}
	bool OutputInterface::isValid() const {
		return mNodeId != FFL::Pipeline_INVALID_Id && mId != FFL::Pipeline_INVALID_Id;
	}

	//
	//  保存，获取trackback信息
	//
	void NodeBase::getTrackbackInfo(int32_t msgType, const FFL::sp<FFL::PipelineMessage> inMsg) {

	}
	void NodeBase::putTrackbackInfo(int32_t msgType, FFL::sp<FFL::PipelineMessage> outMsg, void* user) {

	}
	void NodeBase::resetTrackbackInfo(int32_t msgType) {

	}

}
