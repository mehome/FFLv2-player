/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  NativeFFLPlayer.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/06/28
*  https://github.com/zhenfei2016/FFL-v2.git
*  Native层的FFLPlayer的一个包装层，把一些调用转到player::FFLPlayer中
*
*/
#include "NativeFFLPlayer.hpp"
#include "JavaFFLPlayer.hpp"
#include "Player.hpp"
#include <android/native_window_jni.h>

namespace android{
    //
    //  通过java层对象获取实质指向的native层player指针
    //
    static  player::FFLPlayer* getRealPlayer(JNIEnv* env, jobject thiz){
        NativeFFLPlayer* player=JavaFFLPlayerClass::getNativePlayer(*env,thiz);
        if(player){
            return player->mRealPlayer;
        }
        return NULL;
    }

    class TestListener : public player::IPlayerListener {
    public:
        TestListener(player::FFLPlayer* player) :mPlayer(player) {

        }

        virtual void onPrepared(int suc) {
            printf("onPrepared suc=%d\n", suc);
            if (suc) {
                mPlayer->start();
            }
        }
        //
        // 视频大小改变了
        // widht:宽度 ，height：高度
        // aspectRatioNum/aspectRatioDen  宽高比
        //
        virtual void onVideoSizeChanged(int32_t width, int32_t height, int32_t aspectRatioNum, int32_t aspectRatioDen) {
            printf("onVideoSizeChanged: width=%d height=%d aspectRatio=%d/%d\n",
                   width, height,
                   aspectRatioNum,
                   aspectRatioDen);
            //mPlayer->setSurfaceSize(width,height);
        }
        //
        //  视频播放结束回调
        //
        virtual void onComplete() {
            printf("onComplete:  \n");
        }
        //
        //  seek结束回调
        //
        virtual void onSeekComplete(int64_t pos, int64_t duration) {
            char durationBuf[256] = {};
            FFL_usToString(duration, durationBuf);

            char posBuf[256] = {};
            FFL_usToString(pos, posBuf);
            printf("onSeekComplete: pos=%s duration=%s \n", posBuf, durationBuf);
        }
        //
        // 错误码
        //
        virtual void onError(int32_t errCode, int32_t errVal) {
            printf("onSeekComplete: errCode=%d errVal=%d \n", errCode, errVal);
        }
        //
        // 缓冲中
        //
        virtual void onBufferingStart() {
            printf("onBufferingStart:  \n");
        }
        virtual void onBufferingUpdate() {
            printf("onBufferingUpdate:  \n");
        }
        //
        // 一些其他的消息
        //
        virtual void onMessage(int32_t msg, int32_t parma1, int32_t param2) {
            printf("onMessage: msg=%d param1=%d parma2=%d\n", msg, parma1, param2);
        }

    private:
        player::FFLPlayer* mPlayer;
    };

	NativeFFLPlayer::NativeFFLPlayer(){
        mRealPlayer=new player::FFLPlayer();
	}
    NativeFFLPlayer::~NativeFFLPlayer(){
        FFL_SafeFree(mRealPlayer);
    }
	//
	//  获取导出的native层函数表
	//
	JNINativeMethod* NativeFFLPlayer::getExportMethods(int32_t* count){
		//
		//  native层的函数导出到java层
		//
		static JNINativeMethod nativeMethodTables[] = {
				{ "native_setup","(Ljava/lang/Object;)V",(void *)NativeFFLPlayer::setup },
				{ "native_release","()V",(void *)NativeFFLPlayer::release },
				{ "native_finalize","()V",(void *)NativeFFLPlayer::finalize },
				{ "native_play","(Ljava/lang/String;)I",(void *)NativeFFLPlayer::play },
                { "native_setUrl","(Ljava/lang/String;)I",(void *)NativeFFLPlayer::setUrl },
                { "native_setSurface","(Landroid/view/Surface;)I",(void *)NativeFFLPlayer::setSurface },
                { "native_setSurfaceSize","(II)I",(void *)NativeFFLPlayer::setSurfaceSize },
                { "native_prepare","()I",(void *)NativeFFLPlayer::prepare },
                { "native_start","()I",(void *)NativeFFLPlayer::start },
                { "native_pause","(I)I",(void *)NativeFFLPlayer::pause },
                { "native_stop","()I",(void *)NativeFFLPlayer::stop },
                { "native_seekTo","(J)I",(void *)NativeFFLPlayer::seekTo },
                { "native_getCurrentPosition","()J",(void *)NativeFFLPlayer::getCurrentPosition },
                { "native_getDuration","()J",(void *)NativeFFLPlayer::getDuration },
                { "native_setVolume","(I)V",(void *)NativeFFLPlayer::setVolume },
                { "native_getVolume","()I",(void *)NativeFFLPlayer::getVolume },
                { "native_setLoop","(I)V",(void *)NativeFFLPlayer::setLoop },
                { "native_getLoop","()I",(void *)NativeFFLPlayer::getLoop },
		};

		if(count){
			*count=FFL_ARRAY_ELEMS(nativeMethodTables);
		}
		return nativeMethodTables;
	}
	//
	//  native的函数
	//
	void NativeFFLPlayer::setup(JNIEnv* env, jobject thiz,jobject weakthis){
		NativeFFLPlayer* player=new NativeFFLPlayer();
		JavaFFLPlayerClass::saveNativePlayer(*env,thiz,player);

        jobject globalObj=JNInewGlobalRef(*env,weakthis);
		player->mJavaFFLPlayer=new JavaFFLPlayer(globalObj,true);
        player->mRealPlayer->setListener(new TestListener(player->mRealPlayer));

	}
	void NativeFFLPlayer::release(JNIEnv* env, jobject thiz){

	}
	void NativeFFLPlayer::finalize(JNIEnv* env, jobject thiz){

	}
    //
    //  播放这个url指向的视频
    //
    status_t NativeFFLPlayer::play(JNIEnv* env, jobject thiz,jstring url){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            const char* nativeStr=env->GetStringUTFChars(url,NULL);
            player->setUrl(nativeStr);
            env->ReleaseStringUTFChars(url,nativeStr);
            player->prepare();
        }
        return  FFL_OK;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  设置播放url
    //
    status_t NativeFFLPlayer::setUrl(JNIEnv* env, jobject thiz,jstring url){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            const char* nativeStr=env->GetStringUTFChars(url,NULL);
            player->setUrl(nativeStr);
            env->ReleaseStringUTFChars(url,nativeStr);
        }
        return FFL_OK;
    }
    //
    // 设置渲染到的目标
    //
    status_t NativeFFLPlayer::setSurface(JNIEnv* env, jobject thiz,jobject surface){
        status_t ret=FFL_FAILED;
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            ANativeWindow *nativeWindow = NULL;
            if (surface) {
                nativeWindow = ANativeWindow_fromSurface(env,surface);
                if (!nativeWindow)
                {
                    FFL_LOG_WARNING("Failed to  ANativeWindow_fromSurface", __func__);
                }
            }

            if (nativeWindow) {
                ret=player->setSurface(nativeWindow);
                ANativeWindow_release(nativeWindow);
            }
        }
        return  ret;
    }
    //
    // 设置渲染窗口的大小，只有窗口存在的情况下才可以设置大小，否则返回失败
    //
    status_t NativeFFLPlayer::setSurfaceSize(JNIEnv* env, jobject thiz,jint widht,jint height){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            player->setSurfaceSize(widht,height);
        }
        return FFL_OK;
    }
    //
    //  播放，暂停，停止
    //
    status_t NativeFFLPlayer::prepare(JNIEnv* env, jobject thiz){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            player->prepare();
        }
        return FFL_OK;
    }
    status_t NativeFFLPlayer::start(JNIEnv* env, jobject thiz){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            player->start();
        }
        return FFL_OK;
    }
    //
    //  pause:1 进行暂停， 0：恢复
    //
    status_t NativeFFLPlayer::pause(JNIEnv* env, jobject thiz,jint pause){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            player->pause(pause);
        }
        return FFL_OK;
    }
    status_t NativeFFLPlayer::stop(JNIEnv* env, jobject thiz){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            player->stop();
        }
        return FFL_OK;
    }
    //
    //  定位到指定us处
    //  获取当前的播放位置us
    //  获取总的播放时长us
    //
    status_t NativeFFLPlayer::seekTo(JNIEnv* env, jobject thiz,jlong us){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            player->seekTo(us);
        }
        return FFL_OK;
    }
    jlong  NativeFFLPlayer::getCurrentPosition(JNIEnv* env, jobject thiz){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            return player->getCurrentPosition();
        }
        return -1;
    }
    jlong  NativeFFLPlayer::getDuration(JNIEnv* env, jobject thiz){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            return player->getDuration();
        }
        return -1;
    }
    //
    //  获取，设置播放速度，正常速度=100
    //
    jint NativeFFLPlayer::getSpeed(JNIEnv* env, jobject thiz){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            return player->getSpeed();
        }
        return 100;
    }
    void NativeFFLPlayer::setSpeed(JNIEnv* env, jobject thiz,jint speed){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            player->setSpeed(speed);
        }
    }
    //
    // 获取，设置音量
    //  0-255
    //
    void NativeFFLPlayer::setVolume(JNIEnv* env, jobject thiz,jint volume){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            player->setVolume(volume);
        }
    }
    jint NativeFFLPlayer::getVolume(JNIEnv* env, jobject thiz){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            return  player->getVolume();
        }
        return 100;
    }
    //
    // 获取，设置循环播放次数
    // 如果<0 : 一直循环播放
    //     =0 : 播放一次
    //     >0 : 播放num+1次
    //
    void NativeFFLPlayer::setLoop(JNIEnv* env, jobject thiz,jint num){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            player->setLoop(num);
        }
    }
    jint NativeFFLPlayer::getLoop(JNIEnv* env, jobject thiz){
        player::FFLPlayer* player=getRealPlayer(env,thiz);
        if(player){
            player->getLoop();
        }
        return 0;

    }
    //
    // 获取，设置一些特定的选项
    //
    void NativeFFLPlayer::setOptionString(JNIEnv* env, jobject thiz,jstring name, jstring value){

    }
    void NativeFFLPlayer::getOptionString(JNIEnv* env, jobject thiz,jstring name, char* buf, jint bufSize, jstring defaultVal){

    }
    //
    // 获取，设置一些特定的选项
    //
    void NativeFFLPlayer::setOptionInt64(JNIEnv* env, jobject thiz,jstring name, int64_t value){

    }
    void NativeFFLPlayer::getOptionInt64(JNIEnv* env, jobject thiz,jstring  name, int64_t* value, int64_t defaultVal){

    }
}