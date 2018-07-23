/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  MessageFFMpegFrame.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/03/11
*
*  视频，音频解码后的结构包装
*
*/

#include "MessageFFMpegFrame.hpp"
#include "PlayerConstant.hpp"

namespace message {
	FFMpegVideoFrame::FFMpegVideoFrame():mFrame(NULL)
	{
        setType(MSG_FFMPEG_VIDEO_FRAME);
		
	}
	FFMpegVideoFrame::~FFMpegVideoFrame()
	{
		if (mFrame) {			
			av_frame_unref(mFrame);
			av_frame_free(&mFrame);
		}
	}
	//
	//  已经处理完成了，可以回收了		
	//
	void FFMpegVideoFrame::consume()
	{		
		av_frame_unref(mFrame);
		av_frame_free(&mFrame);
		mFrame = NULL;
	}

	int64_t gLastPts = 0;
	//
	//  填充数据
	//
	void FFMpegVideoFrame::fillAvframe(AVFrame* frame){
		if (mFrame!=NULL && mFrame!=frame) {
			FFL_LOG_WARNING("fillAvframe FFMpegVideoFrame av_frame_unref");			
			av_frame_unref(mFrame);
			av_frame_free(&mFrame);
			mFrame = NULL;
		}

		mFrame = frame;
		mTexture.mWidth = frame->width;
		mTexture.mHeight = frame->height;		
		
		mTexture.mDuration = frame->pkt_duration;
		mTexture.mPts = frame->pts;

		if (mTexture.mPts < gLastPts) {
//			FFL_ASSERT(0);
		}
		gLastPts = mTexture.mPts;
		FFL_LOG_DEBUG_TAG(TAG_TIMESTAMP, "decoder pts=%" lld64 " dts=%" lld64, frame->pts, frame->pkt_dts);
		
		switch (frame->format)
		{
		case AV_PIX_FMT_YUV420P:
			mTexture.mPlanesCount = 3;
			break;

		default:
			mTexture.mPlanesCount = 1;
			break;
		}
		mTexture.mPixFormat = frame->format;
		
		for (int i = 0; i < mTexture.mPlanesCount; i++) {
			mTexture.mPitches[i] = frame->linesize[i];
			mTexture.mPixels[i] = frame->data[i];
		}

		//av_guess_sample_aspect_ratio()
		//mTexture.mSampleAspectRatioDen
	}


	FFMpegAudioFrame::FFMpegAudioFrame() :mFrame(NULL)
	{

        setType(MSG_FFMPEG_AUDIO_FRAME);
	}
	FFMpegAudioFrame::~FFMpegAudioFrame()
	{
		if (mFrame) {
			FFL_LOG_WARNING("FFMpegAudioFrame av_frame_unref");
			av_frame_unref(mFrame);
			av_frame_free(&mFrame);
		}
	}


	//
	//  已经处理完成了，可以回收了		
	//
	void FFMpegAudioFrame::consume()
	{
		mSamples.freeData();
		av_frame_unref(mFrame);
		av_frame_free(&mFrame);
	}

	//
	//  填充数据
	//
	void FFMpegAudioFrame::fillAvframe(AVFrame* frame){
		if (mFrame) {
			FFL_LOG_WARNING("fillAvframe FFMpegAudioFrame av_frame_unref");
		}
		mFrame = frame;

		mSamples.mSampleNum= frame->nb_samples;		
		mSamples.mDuration = frame->pkt_duration;
		mSamples.mPts = frame->pts;
		mSamples.fillData(frame->data, frame->linesize[0], frame->nb_samples);
	}
}
