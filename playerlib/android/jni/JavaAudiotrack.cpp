/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  JavaAudiotrack.hpp
*  Created by zhufeifei(34008081@qq.com) on 2018/06/28
*  https://github.com/zhenfei2016/FFL-v2.git
*
*  java层的AudioTrack用于播放音频
*/
#include "JavaAudiotrack.hpp"

namespace android{
    JavaAudioTrackClass* gJavaAudioTrackClass=NULL;
    //
    //  引用进来audiotrack
    //
    JavaAudioTrackClass* importJavaAudioTrack( JNIEnv& env){
        if(!gJavaAudioTrackClass) {
            gJavaAudioTrackClass = new JavaAudioTrackClass(env);
            gJavaAudioTrackClass->init(env);
        }
        return gJavaAudioTrackClass;
    }
    JavaAudioTrackClass* getJavaAudioTrackClass(){
        return  gJavaAudioTrackClass;
    }


    JavaAudioTrackClass::JavaAudioTrackClass(JNIEnv &env) :
         JavaClass(env,"android/media/AudioTrack") {
    }
    //
    //  进行初始化，反初始化具体逻辑
    //
    bool JavaAudioTrackClass::onInit(JNIEnv& env){
        const char* methodName=NULL;
        const char* methodSign=NULL;

        methodName="<init>";
        methodSign="(IIIIII)V";
        mMethod_constructor=getMethodId(env,methodName,methodSign);

        methodName="play";
        methodSign="()V";
        mMethod_play=getMethodId(env,methodName,methodSign);

        methodName="pause";
        methodSign="()V";
        mMethod_pause=getMethodId(env,methodName,methodSign);

        methodName="flush";
        methodSign="()V";
        mMethod_flush=getMethodId(env,methodName,methodSign);

        methodName="release";
        methodSign="()V";
        mMethod_release=getMethodId(env,methodName,methodSign);

        methodName="write";
        methodSign="([BII)I";
        getMethodId(env,methodName,methodSign);

        methodName="getMinBufferSize";
        methodSign="(III)I";
        getStaticMethodId(env,methodName,methodSign);

        methodName="getMaxVolume";
        methodSign="()F";
        getStaticMethodId(env,methodName,methodSign);

        methodName="getMinVolume";
        methodSign="()F";
        getStaticMethodId(env,methodName,methodSign);

        methodName="getNativeOutputSampleRate";
        methodSign="(I)I";
        getStaticMethodId(env,methodName,methodSign);

        methodName="setStereoVolume";
        methodSign="(FF)I";
        getMethodId(env,methodName,methodSign);

        methodName="getAudioSessionId";
        methodSign="()I";
        getMethodId(env,methodName,methodSign);

        methodName="getStreamType";
        methodSign="()I";
        getMethodId(env,methodName,methodSign);

        methodName="getSampleRate";
        methodSign="()I";
        getMethodId(env,methodName,methodSign);

        methodName="getPlaybackRate";
        methodSign="()I";
        getMethodId(env,methodName,methodSign);

        methodName="setPlaybackRate";
        methodSign="(I)I";
        getMethodId(env,methodName,methodSign);

        return true;
    }

    void JavaAudioTrackClass::onUninit(JNIEnv& env){

    }
    //
    //   创建，删除一个Audiotrack
    //
    JavaAudioTrack* JavaAudioTrackClass::allocAudioTrack(JNIEnv& env,
            jint streamType,
            jint sampleRateInHz,
            jint channelConfig,
            jint audioFormat,
            jint bufferSizeInBytes,
            jint mode){
            jobject javaAudioTrack=env.NewObject(getJClass(), mMethod_constructor, streamType, sampleRateInHz, channelConfig, audioFormat, bufferSizeInBytes, mode);
            return  new JavaAudioTrack(javaAudioTrack);
    }
    void JavaAudioTrackClass::freeAudioTrack(JavaAudioTrack* audiotrack){
    }


    JavaAudioTrack::JavaAudioTrack(jobject javaAudioTrack) :
            JavaObject<JavaAudioTrackClass>(*gJavaAudioTrackClass,javaAudioTrack, false) {
        mJNIEnv=NULL;
    }
    JavaAudioTrack::~JavaAudioTrack(){

    }
    void JavaAudioTrack::play(){
        mJNIEnv->CallVoidMethod(getJavaObject(),getClass().mMethod_play);
    }
    void JavaAudioTrack::pause(){
        mJNIEnv->CallVoidMethod(getJavaObject(),getClass().mMethod_pause);
    }
    void JavaAudioTrack::flush(){
        mJNIEnv->CallVoidMethod(getJavaObject(),getClass().mMethod_flush);
    }
    void JavaAudioTrack::setVolume(float left,float right){

    }
    void JavaAudioTrack::stop(){
        //mJNIEnv->CallVoidMethod(getJavaObject(),getClass().mMethod_stop);
    }
    void JavaAudioTrack::release(){
        mJNIEnv->CallVoidMethod(getJavaObject(),getClass().mMethod_release);
    }
    int JavaAudioTrack::write(uint8_t* data,int size){
        return 0;
    }


}