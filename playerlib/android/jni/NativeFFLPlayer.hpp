/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  NativeFFLPlayer.hpp   
*  Created by zhufeifei(34008081@qq.com) on 2018/06/28 
*  https://github.com/zhenfei2016/FFL-v2.git
*  Native层的FFLPlayer的一个包装层，把一些调用转到player::FFLPlayer中
*
*/

#ifndef _NATIVE_FFL_PLAYER_HPP_
#define _NATIVE_FFL_PLAYER_HPP_
#include <FFL.h>
#include <jni.h>
#include "JavaFFLPlayer.hpp"

namespace  player{
  class FFLPlayer;
};
namespace android {
	class NativeFFLPlayer {
	public:
		NativeFFLPlayer();
		~NativeFFLPlayer();

		//
		//  获取导出的native层函数表
		//
		static JNINativeMethod* getExportMethods(int32_t* count);
	public:
		//
		//  native的函数
		//
		static void setup(JNIEnv* env, jobject thiz,jobject weakthis);
		static void release(JNIEnv* env, jobject thiz);
		static void finalize(JNIEnv* env, jobject thiz);

		//
		//  播放这个url指向的视频
		//
		static status_t play(JNIEnv* env, jobject thiz,jstring url);
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//  设置播放url
		//
		static status_t setUrl(JNIEnv* env, jobject thiz,jstring url);
		//
		// 设置渲染到的目标
		//
		static status_t setSurface(JNIEnv* env, jobject thiz,jobject surface);
		//
		// 设置渲染窗口的大小，只有窗口存在的情况下才可以设置大小，否则返回失败
		//
		static status_t setSurfaceSize(JNIEnv* env, jobject thiz,jint widht,jint height);
		//
		//  播放，暂停，停止
		//
		static status_t prepare(JNIEnv* env, jobject thiz);
		static status_t start(JNIEnv* env, jobject thiz);
		//
		//  pause:1 进行暂停， 0：恢复
		//
		static status_t pause(JNIEnv* env, jobject thiz,jint pause);
		static status_t stop(JNIEnv* env, jobject thiz);
		//
		//  定位到指定us处
		//  获取当前的播放位置us
		//  获取总的播放时长us
		//
		static status_t seekTo(JNIEnv* env, jobject thiz,jlong us);
		static jlong  getCurrentPosition(JNIEnv* env, jobject thiz);
		static jlong  getDuration(JNIEnv* env, jobject thiz);
		//
		//  获取，设置播放速度，正常速度=100
		//
		static jint getSpeed(JNIEnv* env, jobject thiz);
		static void setSpeed(JNIEnv* env, jobject thiz,jint speed);
		//
		// 获取，设置音量
		//  0-255
		//
		static void setVolume(JNIEnv* env, jobject thiz,jint volume);
		static jint getVolume(JNIEnv* env, jobject thiz);
		//
		// 获取，设置循环播放次数
		// 如果<0 : 一直循环播放
		//     =0 : 播放一次
		//     >0 : 播放num+1次
		//
		static void setLoop(JNIEnv* env, jobject thiz,jint num);
		static jint getLoop(JNIEnv* env, jobject thiz);
		//
		// 获取，设置一些特定的选项
		//
		static void setOptionString(JNIEnv* env, jobject thiz,jstring  name, jstring  value);
		static void getOptionString(JNIEnv* env, jobject thiz,jstring  name, char* buf, jint bufSize,jstring  defaultVal);
		//
		// 获取，设置一些特定的选项
		//
		static void setOptionInt64(JNIEnv* env, jobject thiz,jstring  name, int64_t value);
		static void getOptionInt64(JNIEnv* env, jobject thiz,jstring  name, int64_t* value, int64_t defaultVal);
	public:
		//
		//  可以通过这个指针调用java层的函数
		//
		JavaFFLPlayer *mJavaFFLPlayer;

		player::FFLPlayer* mRealPlayer;
	};
}
#endif