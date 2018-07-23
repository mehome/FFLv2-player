/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  NodeBase.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/11
*
*  播放器中节点的基类
*
*/
#pragma once
#include <pipeline/FFL_Pipeline.hpp>
#include <utils/FFL_String.hpp>

namespace player {
	class InputInterface;
	class OutputInterface;
	class PlayerCore;

	class NodeBase : public FFL::RefBase
	{
	public:
		NodeBase();
		virtual ~NodeBase();

		//
		// 开始创建这个节点
		//
		status_t create(PlayerCore* player);
		void destroy();
		bool isCreated() const;

		//
		//  获取设置名称
		//
		void setName(const char* name);
		void getName(FFL::String& name) const;		
		const char* getName();
		//
		//  获取这个node的所有者player
		//
		PlayerCore* getOwner() const;	
		//
		//  获取nodeid
		//
		FFL::PipelineNodeId getNodeId() const;
		//
		//  创建输入接口
		//
		InputInterface createInputInterface(FFL::sp<FFL::PipelineInputHandler> callback, const char* name);
		//
		//   创建输出接口
		//
		OutputInterface createOutputInterface();
        //
        //  获取输入
        //
        FFL::sp<FFL::PipelineInput> getInput(FFL::PipelineInputId id);
		FFL::sp<FFL::PipelineOutput> getOutput(FFL::PipelineOutputId id);
		//
		//  连接一个输出到一个输入
		//
		bool connect(const OutputInterface& output, const InputInterface& input,
			FFL::sp<FFL::PipelineConnector> conn);
		
		//
		//  output:数据提供者输出接口
		//	input：返回当前的这个节点的输入接口接口
		//  返回是否成功
		//
		bool connectSource(const OutputInterface& output,const char* name, InputInterface& input,void* userdata);
	protected:
		friend class FFL::ClassMethodPipelineInputHandler<NodeBase>;
		//
		// 收到消息 ,connectSource建立的连接，连接上的数据来了会触发这个函数
		//
		void receiveData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata);

		//
		//   外部connectSource时候调用此函数，创建对应conn
		//
		virtual FFL::sp<FFL::PipelineConnector > onCreateConnector(
			const OutputInterface& output,
			const InputInterface& input,
			void* userdata);

		FFL::sp<FFL::PipelineConnector> getConnector(const OutputInterface& output);
		FFL::sp<FFL::PipelineConnector> getConnector(const InputInterface& input);

		//
		// 处理接收到的消息，如果返回false表示这个消息没有处理，
		// 返回true表示这个消息处理了，消息处理结束后必须msg->consume();
		//
		virtual bool handleReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) ;
	public:
		//
		//  发送事件消息
		//
		void postEvent(const FFL::sp<FFL::PipelineEvent> &msg);
		//
		//  发送消息到output		
		//  dstId: 目标id
		//  msg : 消息内容
		//
		status_t postMessage(FFL::PipelineOutputId dstId, const FFL::sp<FFL::PipelineMessage> &msg);
		status_t postMessageDelay(FFL::PipelineOutputId dstId, const FFL::sp<FFL::PipelineMessage> &msg, uint64_t delayUs);
		//
		//  给当前节点的一个输入接口发送一条消息
		//
		status_t postSelfMessage(FFL::PipelineInputId dstId, const FFL::sp<FFL::PipelineMessage> &msg, uint64_t delayUs);
	protected:
		//
		//   清空输出节点中没有处理的消息
		//
		void clearMessage(FFL::PipelineInputId dstId);

	protected:
		//
		//  保存，获取trackback信息
		//
		void getTrackbackInfo(int32_t msgType,const FFL::sp<FFL::PipelineMessage> inMsg);
		void putTrackbackInfo(int32_t msgType,FFL::sp<FFL::PipelineMessage> outMsg,void* user);
		void resetTrackbackInfo(int32_t msgType);

	protected:
		//
		//  成功创建了node
		//
		virtual void onCreate();
		//
		//  成功删除了node
		//
		virtual void onDestroy();
		virtual void onStartMessageLooper();
		virtual void onQuitMessageLooper();
		
		//
		//  获取这个节点属于哪一个线的
		//
		FFL::sp<FFL::Pipeline> getPipeline();
	private:
		//
		//  所属的播放器
		//
		PlayerCore* mPlayer;
		//
		// 节点id
		//
		FFL::PipelineNodeId mNodeId;
		//
		// 节点的名称
		//
		FFL::String mNodeName;
		//
		//  pipeline系统中的处理句柄
		//
		class NodeHandler;
		FFL::sp<NodeHandler> mNodeHandler;
	};

	//
	//  Node的输入接口
	//
	class InputInterface {
	public:
		InputInterface();
		InputInterface(FFL::PipelineNodeId nodeId, FFL::PipelineInputId inputId);
		bool isValid() const;

		FFL::PipelineNodeId  mNodeId;
		FFL::PipelineInputId mId;
	};

	//
	//  就是Node的输出接口
	//
	class OutputInterface {
	public:
		OutputInterface();
		OutputInterface(FFL::PipelineNodeId nodeId, FFL::PipelineOutputId outputId);
		bool isValid() const;

		void reset();

		FFL::PipelineNodeId  mNodeId;
		FFL::PipelineOutputId mId;
		FFL::String mName;
	};
}
